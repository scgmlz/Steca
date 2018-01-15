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
#include "core/data/suite.h"
#include "core/data/measurement.h"
#include "core/fit/peak_functions.h"

Session::Session()
    : intenScale_(1)
    , angleMapCache_(360)
{
    clear();
    register_peak_functions();
}

void Session::clear() {
    while (numFiles())
        removeFile(0);

    removeCorrFile();
    corrEnabled_ = corrHasNaNs_ = false;

    bgPolyDegree_ = 0;
    bgRanges_.clear();

    reflections_.clear();

    norm_ = eNorm::NONE;

    angleMapCache_.clear();

    intenScaledAvg_ = true;
    intenScale_ = 1;
}

bool Session::hasFile(rcstr fileName) const {
    QFileInfo fileInfo(fileName);
    for (const shp_Datafile& file : files_)
        if (fileInfo == file->fileInfo())
            return true;
    return false;
}

void Session::addGivenFile(shp_Datafile datafile) THROWS {
    setImageSize(datafile->imageSize());
    // all ok
    files_.append(datafile);
}

void Session::removeFile(int i) {
    files_.remove(i);
    updateImageSize();
}

void Session::calcIntensCorr() const {
    corrHasNaNs_ = false;

    debug::ensure(corrImage_);
    size2d size = corrImage_->size() - imageCut_.marginSize();
    debug::ensure(!size.isEmpty());

    int w = size.w, h = size.h, di = imageCut_.left, dj = imageCut_.top;

    qreal sum = 0;
    for_ij (w, h)
        sum += corrImage_->inten(i + di, j + dj);
    qreal avg = sum / (w * h);

    intensCorr_.fill(1, corrImage_->size());

    for_ij (w, h) {
        const inten_t inten = corrImage_->inten(i + di, j + dj);
        qreal fact;
        if (inten > 0) {
            fact = avg / inten;
        } else {
            fact = NAN;
            corrHasNaNs_ = true;
        }
        intensCorr_.setInten(i + di, j + dj, inten_t(fact));
    }
}

const Image* Session::intensCorr() const {
    if (!isCorrEnabled())
        return nullptr;
    if (intensCorr_.isEmpty())
        calcIntensCorr();
    return &intensCorr_;
}

void Session::setCorrFile(shp_Datafile datafile) THROWS {
    if (datafile.isNull()) {
        removeCorrFile();
    } else {
        setImageSize(datafile->imageSize());
        corrImage_ = datafile->foldedImage();
        intensCorr_.clear(); // will be calculated lazily
        // all ok
        corrFile_ = datafile;
        corrEnabled_ = true;
    }
}

void Session::removeCorrFile() {
    corrFile_.clear();
    corrImage_.clear();
    intensCorr_.clear();
    corrEnabled_ = false;
    updateImageSize();
}

void Session::collectDatasetsFromFiles(int_vec fileNums, int combineBy) {

    collectedFromFiles_ = fileNums;
    experiment_.clear();
    experimentTags_.clear();

    vec<shp_Measurement> suiteFromFiles;
    for (int i : collectedFromFiles_)
        for (const shp_Measurement& measurement : files_.at(i)->suite())
            suiteFromFiles.append(measurement);
    if (suiteFromFiles.isEmpty())
        return;

    shp_Suite cd(new Suite);
    int i = 0;

    auto _appendCd = [this, &cd, &combineBy, &i]() {
        int cnt = cd->count();
        if (cnt) {
            str tag = str::number(i + 1);
            i += cnt;
            if (combineBy > 1)
                tag += '-' + str::number(i);
            experiment_.appendHere(cd);
            experimentTags_.append(tag);
            cd = shp_Suite(new Suite);
        }
    };

    int by = combineBy;
    for (const shp_Measurement& measurement : suiteFromFiles) {
        cd->append(measurement);
        if (1 >= by--) {
            _appendCd();
            by = combineBy;
        }
    }

    _appendCd(); // the remaining ones
}

void Session::updateImageSize() {
    if (0 == numFiles() && !hasCorrFile())
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
    intensCorr_.clear(); // lazy
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

shp_SequenceLens Session::dataseqLens(Suite const& suite, eNorm norm, bool trans, bool cut) const {
    return shp_SequenceLens(new SequenceLens(suite, norm, trans, cut, imageTransform_, imageCut_));
}

shp_SequenceLens Session::defaultDataseqLens(Suite const& suite) const {
    return dataseqLens(suite, norm_, true, true);
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
    Suite const& suite = lens.suite();
    suite.calculateAlphaBeta(rgeTth.center(), gmaSector.center(), alpha, beta);

    shp_Metadata metadata = suite.metadata();

    return rgeTth.contains(peak.x)
        ? ReflectionInfo(
              metadata, alpha, beta, gmaSector, inten_t(peak.y), inten_t(peakError.y),
              deg(peak.x), deg(peakError.x), fwhm_t(fwhm), fwhm_t(fwhmError))
        : ReflectionInfo(metadata, alpha, beta, gmaSector);
}

//! Gathers ReflectionInfos from Datasets.

//! Either uses the whole gamma range of the suite (if gammaSector is invalid),
//!  or user limits the range.
//! Even though the betaStep of the equidistant polefigure grid is needed here,
//!  the returned infos won't be on the grid.
//! TODO? gammaStep separately?

ReflectionInfos Session::makeReflectionInfos(
    Experiment const& expt, Reflection const& reflection, int gmaSlices,
    const Range& rgeGma, Progress* progress) const {

    ReflectionInfos infos;

    if (progress)
        progress->setTotal(expt.count());

    for (const shp_Suite& suite : expt) {
        if (progress)
            progress->step();

        const shp_SequenceLens& lens = dataseqLens(*suite, norm_, true, true);

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
                infos.append(refInfo);
        }
    }

    return infos;
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

qreal Session::calcAvgBackground(Suite const& suite) const {
    const shp_SequenceLens& lens = dataseqLens(suite, eNorm::NONE, true, true);
    Curve gmaCurve = lens->makeCurve(); // had argument averaged=true
    Polynom bgPolynom = Polynom::fromFit(bgPolyDegree_, gmaCurve, bgRanges_);
    return bgPolynom.avgY(lens->rgeTth());
}

qreal Session::calcAvgBackground() const {
    TakesLongTime __;
    qreal bg = 0;
    for (const shp_Suite& suite : experiment())
        bg += calcAvgBackground(*suite);
    return bg / experiment().count();
}
