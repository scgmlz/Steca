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

#include "core/session.h"
#include "core/fit/peak_functions.h"

Session::Session()
    : intenScale_(1)
    , angleMapCache_(360)
{
    clear();
    register_peak_functions();
}

void Session::clear() {
    dataset_.clear();
    corrset_.clear();

    bgPolyDegree_ = 0;
    bgRanges_.clear();

    reflections_.clear();

    norm_ = eNorm::NONE;

    angleMapCache_.clear();

    intenScaledAvg_ = true;
    intenScale_ = 1;
}

void Session::setMetaSelection(const vec<bool>& metaSelection) {
    metaSelection_ = metaSelection;
    emit gSession->sigMetaSelection();
}

void Session::updateImageSize() {
    if (0 == dataset().countFiles() && !corrset().hasFile())
        imageSize_ = size2d(0, 0);
}

void Session::setImageSize(size2d const& size) THROWS {
    RUNTIME_CHECK(!size.isEmpty(), "image is empty or has ill defined size");
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
}

void Session::setGeometry(qreal detectorDistance, qreal pixSize, IJ const& midPixOffset) {
    geometry_.detectorDistance = detectorDistance;
    geometry_.pixSize = pixSize;
    geometry_.midPixOffset = midPixOffset;
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
    return dataseqLens(*dataset().highlightedCluster(), norm_, true, true);
}

Curve Session::curveMinusBg(SequenceLens const& lens, const Range& rgeGma) const {
    Curve curve = lens.makeCurve(rgeGma);
    const Polynom f = Polynom::fromFit(bgPolyDegree_, curve, bgRanges_);
    curve.subtract([f](qreal x) {return f.y(x);});
    return curve;
}

//! Fits reflection to the given gamma sector and constructs a ReflectionInfo.
ReflectionInfo Session::makeReflectionInfo(
    SequenceLens const& lens, Reflection const& reflection, const Range& gmaSector) const {

    // fit peak, and retrieve peak parameters:
    Curve curve = curveMinusBg(lens, gmaSector);
    scoped<PeakFunction*> peakFunction = FunctionRegistry::clone(reflection.peakFunction());
    peakFunction->fit(curve);
    const Range& rgeTth = peakFunction->range();
    qpair peak = peakFunction->fittedPeak();
    fwhm_t fwhm = peakFunction->fittedFWHM();
    qpair peakError = peakFunction->peakError();
    fwhm_t fwhmError = peakFunction->fwhmError();

    // compute alpha, beta:
    deg alpha, beta;
    Sequence const& seq = lens.sequence();
    seq.calculateAlphaBeta(rgeTth.center(), gmaSector.center(), alpha, beta);

    shp_Metadata metadata = seq.avgeMetadata();

    return rgeTth.contains(peak.x)
        ? ReflectionInfo(
              metadata, alpha, beta, gmaSector, inten_t(peak.y), inten_t(peakError.y),
              deg(peak.x), deg(peakError.x), fwhm_t(fwhm), fwhm_t(fwhmError))
        : ReflectionInfo(metadata, alpha, beta, gmaSector);
}

//! Gathers ReflectionInfos from Datasets.

//! Either uses the whole gamma range of the cluster (if gammaSector is invalid),
//!  or user limits the range.
//! Even though the betaStep of the equidistant polefigure grid is needed here,
//!  the returned infos won't be on the grid.
//! TODO? gammaStep separately?

ReflectionInfos Session::makeReflectionInfos(
    Reflection const& reflection, int gmaSlices, const Range& rgeGma, Progress* progress) const {

    if (progress)
        progress->setTotal(experiment().count());

    ReflectionInfos ret;

    for (const shp_Cluster& cluster : experiment()) {
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
            const ReflectionInfo refInfo = makeReflectionInfo(*lens, reflection, gmaStripe);
            if (!qIsNaN(refInfo.inten()))
                ret.append(refInfo);
        }
    }

    return ret;
}

void Session::setIntenScaleAvg(bool avg, qreal scale) {
    intenScaledAvg_ = avg;
    intenScale_ = scale;
}

void Session::addReflection(const QString& peakFunctionName) {
    shp_Reflection reflection(new Reflection(peakFunctionName));
    debug::ensure(!reflection.isNull());
    reflections_.append(reflection);
}

void Session::addReflection(const QJsonObject& obj) {
    shp_Reflection reflection(new Reflection);
    reflection->from_json(obj);
    reflections_.append(reflection);
}

qreal Session::calcAvgBackground(Sequence const& seq) const {
    const shp_SequenceLens& lens = dataseqLens(seq, eNorm::NONE, true, true);
    Curve gmaCurve = lens->makeCurve(); // had argument averaged=true
    Polynom bgPolynom = Polynom::fromFit(bgPolyDegree_, gmaCurve, bgRanges_);
    return bgPolynom.avgY(lens->rgeTth());
}

qreal Session::calcAvgBackground() const {
    TakesLongTime __;
    qreal bg = 0;
    for (const shp_Cluster& cluster : experiment())
        bg += calcAvgBackground(*cluster);
    return bg / experiment().count();
}
