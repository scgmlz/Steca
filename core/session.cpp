// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/session.cpp
//! @brief     Implements class Session
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

// This needs to be placed once, somewhere, in our sources:
#ifdef QT_NO_EXCEPTIONS
#error needs exception handling
#endif

#include "core/session.h"
#include "core/fit/peak_functions.h"
#include <QDir>
#include <QJsonDocument>

Session* gSession; //!< global, for data handling

Session::Session()
    : metaSelection_( std::vector<bool>(Metadata::size(), false) )
{
    gSession = this;

    register_peak_functions();

    // Some signals imply other signals:
    connect(this, &Session::sigGamma, this, &Session::sigDiffractogram);
    connect(this, &Session::sigDataHighlight, &gammaSelection_, &GammaSelection::onData);
    connect(this, &Session::sigDataHighlight, &thetaSelection_, &ThetaSelection::onData);
    connect(this, &Session::sigDataHighlight, this, &Session::sigImage);
    connect(this, &Session::sigDetector, this, &Session::sigImage);
    connect(this, &Session::sigNorm, this, &Session::sigImage);
}

void Session::clear() {
    dataset_.clear();
    corrset_.clear();
    baseline_.clear();
    peaks_.clear();

    norm_ = eNorm::NONE;

    angleMapCache_.clear();

    intenScaledAvg_ = true;
    intenScale_ = 1;
}

QByteArray Session::serializeSession() const
{
    QJsonObject top;

    top.insert("dataset", dataset().toJson());
    top.insert("corrset", corrset().toJson());

    const Geometry& geo = geometry();
    QJsonObject sub {
        { "distance", QJsonValue(geo.detectorDistance()) },
        { "pixel size", QJsonValue(geo.pixSize()) },
        { "beam offset", geo.midPixOffset().to_json() }
    };
    top.insert("detector", sub);

    const ImageCut& cut = imageCut();
    sub = {
        { "left", cut.left() },
        { "top", cut.top() },
        { "right", cut.right() },
        { "bottom", cut.bottom() } };
    top.insert("cut", sub);

    // TODO serialize image rotation and mirror



    // TODO save cluster selection

    top.insert("baseline", baseline().toJson());
    top.insert("peaks", peaks().toJson());

    top.insert("averaged intensity ", intenScaledAvg());
    top.insert("intensity scale", qreal_to_json((qreal)intenScale()));

    return QJsonDocument(top).toJson();
}

void Session::sessionFromJson(const QByteArray& json) THROWS
{
    QJsonParseError parseError;
    QJsonDocument doc(QJsonDocument::fromJson(json, &parseError));
    if (!(QJsonParseError::NoError == parseError.error))
        THROW("Error parsing session file");


    clear();
    TR("sessionFromJson: cleared old session");

    JsonObj top(doc.object());

    dataset().fromJson(top.loadObj("dataset"));
    corrset().fromJson(top.loadObj("corrset"));

    const JsonObj& det = top.loadObj("detector");
    geometry().setDetectorDistance(det.loadPreal("distance"));
    geometry().setPixSize(det.loadPreal("pixel size"));
    geometry().setOffset(det.loadIJ("beam offset"));

    TR("sessionFromJson: going to load image cut");
    const JsonObj& cut = top.loadObj("cut");
    int x1 = cut.loadUint("left"), y1 = cut.loadUint("top"),
         x2 = cut.loadUint("right"), y2 = cut.loadUint("bottom");
    imageCut().setLinked(false);
    imageCut().setLeft(x1);
    imageCut().setRight(x2);
    imageCut().setTop(y1);
    imageCut().setBottom(y2);

    TR("sessionFromJson: going to load fit setup");
    baseline().fromJson(top.loadObj("baseline"));

    bool arg1 = top.loadBool("averaged intensity ", true);
    qreal arg2 = top.loadPreal("intensity scale", 1);
    setIntenScaleAvg(arg1, arg2);

    TR("sessionFromJson: going to load peaks info");
    const QJsonArray& peaksInfo = top.loadArr("peaks");
    for_i (peaksInfo.count())
        peaks().add(peaksInfo.at(i).toObject());

    TR("installed session from file");
}

void Session::setMetaSelected(int i, bool on) {
    metaSelection_[i] = on;
    emit gSession->sigMetaSelection();
}

void Session::updateImageSize() {
    if (0 == dataset().countFiles() && !corrset().hasFile())
        imageSize_ = size2d(0, 0);
}

void Session::setImageSize(const size2d& size) THROWS {
    if (!(!size.isEmpty())) THROW("image is empty or has ill defined size");
    if (imageSize_.isEmpty())
        imageSize_ = size; // the first one
    else if (imageSize_ != size)
        THROW("image size differs from previously loaded images");
}

size2d Session::imageSize() const {
    return imageTransform_.isTransposed() ? imageSize_.transposed() : imageSize_;
}

void Session::setImageTransformMirror(bool on) {
    imageTransform_ = imageTransform_.mirror(on);
}

void Session::setImageTransformRotate(const ImageTransform& rot) {
    imageTransform_ = imageTransform_.rotateTo(rot);
}

IJ Session::midPix() const {
    size2d sz = imageSize();
    IJ mid(sz.w / 2, sz.h / 2);

    const IJ& off = geometry_.midPixOffset();
    mid.i += off.i;
    mid.j += off.j;

    return mid;
}

shp_AngleMap Session::angleMap(const Measurement& one) const {
    ImageKey key(geometry_, imageSize_, imageCut_, midPix(), one.midTth());
    shp_AngleMap map = angleMapCache_.value(key);
    if (map.isNull())
        map = angleMapCache_.insert(key, shp_AngleMap(new AngleMap(key)));
    return map;
}

//! Fits peak to the given gamma sector and constructs a PeakInfo.
PeakInfo Session::makePeakInfo(const Cluster* cluster, const qreal normFactor,
                               const Peak& peak, const Range& gmaSector) const {

    // fit peak, and retrieve peak parameters:
    Curve curve = cluster->toCurve(normFactor, gmaSector);
    const Polynom f = Polynom::fromFit(baseline().polynomDegree(), curve, baseline().ranges());
    curve.subtract([f](qreal x) {return f.y(x);});

    std::unique_ptr<PeakFunction> peakFunction( FunctionRegistry::clone(peak.peakFunction()) );
    peakFunction->fit(curve);
    const Range& rgeTth = peakFunction->range();
    qpair fitresult = peakFunction->fittedPeak();
    fwhm_t fwhm = peakFunction->fittedFWHM();
    qpair peakError = peakFunction->peakError();
    fwhm_t fwhmError = peakFunction->fwhmError();

    // compute alpha, beta:
    deg alpha, beta;
    cluster->calculateAlphaBeta(rgeTth.center(), gmaSector.center(), alpha, beta);

    shp_Metadata metadata = cluster->avgeMetadata();

    return rgeTth.contains(fitresult.x)
        ? PeakInfo(
              metadata, alpha, beta, gmaSector, inten_t(fitresult.y), inten_t(peakError.y),
              deg(fitresult.x), deg(peakError.x), fwhm_t(fwhm), fwhm_t(fwhmError))
        : PeakInfo(metadata, alpha, beta, gmaSector);
}

//! Gathers PeakInfos from Datasets.

//! Either uses the whole gamma range of the cluster (if gammaSector is invalid),
//!  or user limits the range.
//! Even though the betaStep of the equidistant polefigure grid is needed here,
//!  the returned infos won't be on the grid.
//! TODO? gammaStep separately?

PeakInfos Session::makePeakInfos(
    const Peak& peak, int gmaSlices, const Range& rgeGma, Progress* progress) const {

    if (progress)
        progress->setTotal(experiment().size());

    PeakInfos ret;

    for (const Cluster* cluster : experiment().clusters()) {
        if (progress)
            progress->step();

        const qreal normFactor = cluster->normFactor();

        Range rge = (gmaSlices > 0) ? cluster->rgeGma() : cluster->rgeGmaFull();
        if (rgeGma.isValid())
            rge = rge.intersect(rgeGma);
        if (rge.isEmpty())
            continue;

        gmaSlices = qMax(1, gmaSlices);
        qreal step = rge.width() / gmaSlices;
        for_i (int(gmaSlices)) {
            qreal min = rge.min + i * step;
            Range gmaStripe(min, min + step);
            const PeakInfo refInfo = makePeakInfo(cluster, normFactor, peak, gmaStripe);
            if (!qIsNaN(refInfo.inten()))
                ret.append(refInfo);
        }
    }

    return ret;
}

// TODO: split into two functions (see usage in panel_diff..)
void Session::setIntenScaleAvg(bool avg, qreal scale) {
    intenScaledAvg_ = avg;
    intenScale_ = scale;
    emit gSession->sigNorm();
}

void Session::setNorm(eNorm norm) {
    norm_ = norm;
    emit gSession->sigNorm();
}

qreal Session::calcAvgBackground(const Sequence& seq) const {
    Curve gmaCurve = seq.toCurve(1.);
    Polynom bgPolynom = Polynom::fromFit(baseline().polynomDegree(), gmaCurve, baseline().ranges());
    return bgPolynom.avgY(seq.rgeTth());
}

qreal Session::calcAvgBackground() const {
    TakesLongTime __;
    qreal bg = 0;
    for (const Cluster* cluster : experiment().clusters())
        bg += calcAvgBackground(*cluster);
    return bg / experiment().size();
}
