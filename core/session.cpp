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

Session* gSession; //!< global, for data handling

Session::Session()
    : metaSelection_( std::vector<bool>(Metadata::size(), false) )
{
    register_peak_functions();
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

void Session::setMetaSelected(int i, bool on) {
    metaSelection_[i] = on;
    emit gSession->sigMetaSelection();
}

void Session::updateImageSize() {
    if (0 == dataset().countFiles() && !corrset().hasFile())
        imageSize_ = size2d(0, 0);
}

void Session::setImageSize(size2d const& size) THROWS {
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

void Session::setImageTransformRotate(ImageTransform const& rot) {
    imageTransform_ = imageTransform_.rotateTo(rot);
}

void Session::setImageCut(bool isTopOrLeft, bool linked, ImageCut const& cut) {
    imageCut_.update(isTopOrLeft, linked, cut, imageSize_);
    corrset().clearIntens(); // lazy
    emit sigDetector();
}

void Session::setGeometry(qreal detectorDistance, qreal pixSize, IJ const& midPixOffset) {
    geometry_.detectorDistance = detectorDistance;
    geometry_.pixSize = pixSize;
    geometry_.midPixOffset = midPixOffset;
    emit sigDetector();
}

void Session::setGammaRange(const Range& r) {
    gammaRange_ = r;
    emit sigDiffractogram();
}

IJ Session::midPix() const {
    size2d sz = imageSize();
    IJ mid(sz.w / 2, sz.h / 2);

    IJ const& off = geometry_.midPixOffset;
    mid.i += off.i;
    mid.j += off.j;

    return mid;
}

shp_AngleMap Session::angleMap(Measurement const& one) const {
    ImageKey key(geometry_, imageSize_, imageCut_, midPix(), one.midTth());
    shp_AngleMap map = angleMapCache_.value(key);
    if (map.isNull())
        map = angleMapCache_.insert(key, shp_AngleMap(new AngleMap(key)));
    return map;
}

shp_ImageLens Session::imageLens(const Image& image, bool trans, bool cut) const {
    return shp_ImageLens(new ImageLens(image, trans, cut));
}

shp_SequenceLens Session::dataseqLens(Sequence const& seq, eNorm norm, bool trans, bool cut) const {
    return shp_SequenceLens(new SequenceLens(seq, norm, trans, cut, imageTransform_, imageCut_));
}

shp_SequenceLens Session::defaultClusterLens(Sequence const& seq) const {
    return dataseqLens(seq, norm_, true, true);
}

shp_SequenceLens Session::highlightsLens() const {
    return dataseqLens(*dataset().highlight().cluster(), norm_, true, true);
}

Curve Session::curveMinusBg(SequenceLens const& lens, const Range& rgeGma) const {
    Curve curve = lens.makeCurve(rgeGma);
    const Polynom f = Polynom::fromFit(baseline().polynomDegree(), curve, baseline().ranges());
    curve.subtract([f](qreal x) {return f.y(x);});
    return curve;
}

//! Fits peak to the given gamma sector and constructs a PeakInfo.
PeakInfo Session::makePeakInfo(
    SequenceLens const& lens, Peak const& peak, const Range& gmaSector) const {

    // fit peak, and retrieve peak parameters:
    Curve curve = curveMinusBg(lens, gmaSector);
    std::unique_ptr<PeakFunction> peakFunction( FunctionRegistry::clone(peak.peakFunction()) );
    peakFunction->fit(curve);
    const Range& rgeTth = peakFunction->range();
    qpair fitresult = peakFunction->fittedPeak();
    fwhm_t fwhm = peakFunction->fittedFWHM();
    qpair peakError = peakFunction->peakError();
    fwhm_t fwhmError = peakFunction->fwhmError();

    // compute alpha, beta:
    deg alpha, beta;
    Sequence const& seq = lens.sequence();
    seq.calculateAlphaBeta(rgeTth.center(), gmaSector.center(), alpha, beta);

    shp_Metadata metadata = seq.avgeMetadata();

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
    Peak const& peak, int gmaSlices, const Range& rgeGma, Progress* progress) const {

    if (progress)
        progress->setTotal(experiment().size());

    PeakInfos ret;

    for (const Cluster* cluster : experiment().clusters()) {
        if (progress)
            progress->step();

        const shp_SequenceLens& lens = dataseqLens(*cluster, norm_, true, true);

        Range rge = (gmaSlices > 0) ? lens->rgeGma() : lens->rgeGmaFull();
        if (rgeGma.isValid())
            rge = rge.intersect(rgeGma);
        if (rge.isEmpty())
            continue;

        gmaSlices = qMax(1, gmaSlices);
        qreal step = rge.width() / gmaSlices;
        for_i (int(gmaSlices)) {
            qreal min = rge.min + i * step;
            Range gmaStripe(min, min + step);
            const PeakInfo refInfo = makePeakInfo(*lens, peak, gmaStripe);
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

qreal Session::calcAvgBackground(Sequence const& seq) const {
    const shp_SequenceLens& lens = dataseqLens(seq, eNorm::NONE, true, true);
    Curve gmaCurve = lens->makeCurve(); // had argument averaged=true
    Polynom bgPolynom = Polynom::fromFit(baseline().polynomDegree(), gmaCurve, baseline().ranges());
    return bgPolynom.avgY(lens->rgeTth());
}

qreal Session::calcAvgBackground() const {
    TakesLongTime __;
    qreal bg = 0;
    for (const Cluster* cluster : experiment().clusters())
        bg += calcAvgBackground(*cluster);
    return bg / experiment().size();
}
