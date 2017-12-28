// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/session.cpp
//! @brief     Implements class Session
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "session.h"

void register_fit_functions();

using typ::size2d;
using typ::vec;
using typ::Image;
using typ::ImageTransform;
using typ::ImageCut;
using typ::Curve;
using typ::Range;

Session::Session() : intenScale_(1), angleMapCache_(360) {
    clear();
    register_fit_functions();
}

void Session::clear() {
    while (0 < numFiles())
        remFile(0);

    remCorrFile();
    corrEnabled_ = corrHasNaNs_ = false;

    bgPolyDegree_ = 0;
    bgRanges_.clear();

    reflections_.clear();

    norm_ = eNorm::NONE;

    angleMapCache_.clear();

    intenScaledAvg_ = true;
    intenScale_ = preal(1);
}

bool Session::hasFile(rcstr fileName) const {
    QFileInfo fileInfo(fileName);
    for (auto& file : files_)
        if (fileInfo == file->fileInfo())
            return true;
    return false;
}

void Session::addGivenFile(data::shp_File file) THROWS {
    setImageSize(file->datasets().imageSize());
    // all ok
    files_.append(file);
}

void Session::remFile(uint i) {
    files_.remove(i);
    updateImageSize();
}

void Session::calcIntensCorr() const {
    corrHasNaNs_ = false;

    debug::ensure(corrImage_);
    size2d size = corrImage_->size() - imageCut_.marginSize();
    debug::ensure(!size.isEmpty());

    uint w = size.w, h = size.h, di = imageCut_.left, dj = imageCut_.top;

    qreal sum = 0;
    for_ij (w, h)
        sum += corrImage_->inten(i + di, j + dj);
    qreal avg = sum / (w * h);

    intensCorr_.fill(1, corrImage_->size());

    for_ij (w, h) {
        auto inten = corrImage_->inten(i + di, j + dj);
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

Image const* Session::intensCorr() const {
    if (!isCorrEnabled())
        return nullptr;
    if (intensCorr_.isEmpty())
        calcIntensCorr();
    return &intensCorr_;
}

void Session::setCorrFile(data::shp_File file) THROWS {
    if (file.isNull()) {
        remCorrFile();
    } else {
        auto& datasets = file->datasets();
        setImageSize(datasets.imageSize());
        corrImage_ = datasets.foldedImage();
        intensCorr_.clear(); // will be calculated lazily
        // all ok
        corrFile_ = file;
        corrEnabled_ = true;
    }
}

void Session::remCorrFile() {
    corrFile_.clear();
    corrImage_.clear();
    intensCorr_.clear();
    corrEnabled_ = false;
    updateImageSize();
}

void Session::collectDatasetsFromFiles(uint_vec fileNums, pint combineBy) {

    collectedFromFiles_ = fileNums;
    collectedDatasets_.clear();
    collectedDatasetsTags_.clear();

    vec<data::shp_OneDataset> datasetsFromFiles;
    for (uint i : collectedFromFiles_)
        for (auto& dataset : files_.at(i)->datasets())
            datasetsFromFiles.append(dataset);

    if (datasetsFromFiles.isEmpty())
        return;

    data::shp_Dataset cd(new data::Dataset);
    uint i = 0;

    auto appendCd = [this, &cd, &combineBy, &i]() {
        uint cnt = cd->count();
        if (cnt) {
            str tag = str::number(i + 1);
            i += cnt;
            if (combineBy > 1)
                tag += '-' + str::number(i);
            collectedDatasets_.appendHere(cd);
            collectedDatasetsTags_.append(tag);
            cd = data::shp_Dataset(new data::Dataset);
        }
    };

    uint by = combineBy;
    for (auto& dataset : datasetsFromFiles) {
        cd->append(data::shp_OneDataset(dataset));
        if (1 >= by--) {
            appendCd();
            by = combineBy;
        }
    }

    appendCd(); // the remaining ones
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

void Session::setGeometry(preal detectorDistance, preal pixSize, typ::IJ const& midPixOffset) {

    geometry_.detectorDistance = detectorDistance;
    geometry_.pixSize = pixSize;
    geometry_.midPixOffset = midPixOffset;
}

typ::IJ Session::midPix() const {
    auto sz = imageSize();
    typ::IJ mid(sz.w / 2, sz.h / 2);

    typ::IJ const& off = geometry_.midPixOffset;
    mid.i += off.i;
    mid.j += off.j;

    return mid;
}

typ::shp_AngleMap Session::angleMap(data::OneDataset const& one) const {
    typ::AngleMap::Key key(geometry_, imageSize_, imageCut_, midPix(), one.midTth());
    typ::shp_AngleMap map = angleMapCache_.value(key);
    if (map.isNull())
        map = angleMapCache_.insert(key, typ::shp_AngleMap(new typ::AngleMap(key)));
    return map;
}

calc::shp_ImageLens Session::imageLens(
    Image const& image, data::Datasets const& datasets, bool trans, bool cut) const {
    return calc::shp_ImageLens(new calc::ImageLens(*this, image, datasets, trans, cut));
}

calc::shp_DatasetLens Session::datasetLens(
    data::Dataset const& dataset, data::Datasets const& datasets, eNorm norm, bool trans, bool cut
    ) const {
    return calc::shp_DatasetLens(new calc::DatasetLens(
        *this, dataset, datasets, norm, trans, cut, imageTransform_, imageCut_));
}

Curve Session::curveMinusBg(calc::DatasetLens const& lens, typ::Range const& rgeGma) const {
    Curve curve = lens.makeCurve(rgeGma);
    curve.subtract(Polynom::fromFit(bgPolyDegree_, curve, bgRanges_));
    return curve;
}

//! Fits reflection to the given gamma sector and constructs a ReflectionInfo.
calc::ReflectionInfo Session::makeReflectionInfo(
    calc::DatasetLens const& lens, calc::Reflection const& reflection,
    typ::Range const& gmaSector) const {

    // fit peak, and retrieve peak parameters:
    Curve curve = curveMinusBg(lens, gmaSector);
    scoped<PeakFunction*> peakFunction = FunctionRegistry::clone(reflection.peakFunction());
    peakFunction->fit(curve);
    Range const& rgeTth = peakFunction->range();
    qpair peak = peakFunction->fittedPeak();
    fwhm_t fwhm = peakFunction->fittedFWHM();
    qpair peakError = peakFunction->peakError();
    fwhm_t fwhmError = peakFunction->fwhmError();

    // compute alpha, beta:
    deg alpha, beta;
    data::Dataset const& dataset = lens.dataset();
    dataset.calculateAlphaBeta(rgeTth.center(), gmaSector.center(), alpha, beta);

    data::shp_Metadata metadata = dataset.metadata();

    return rgeTth.contains(peak.x)
        ? calc::ReflectionInfo(
              metadata, alpha, beta, gmaSector, inten_t(peak.y), inten_t(peakError.y),
              deg(peak.x), deg(peakError.x), fwhm_t(fwhm), fwhm_t(fwhmError))
        : calc::ReflectionInfo(metadata, alpha, beta, gmaSector);
}

/* Gathers ReflectionInfos from Datasets.
 * Either uses the whole gamma range of the datasets (if gammaSector is
 * invalid), or user limits the range.
 * Even though the betaStep of the equidistant polefigure grid is needed here,
 * the returned infos won't be on the grid. REVIEW gammaStep separately?
 */
calc::ReflectionInfos Session::makeReflectionInfos(
    data::Datasets const& datasets, calc::Reflection const& reflection, uint gmaSlices,
    typ::Range const& rgeGma, Progress* progress) const {
    calc::ReflectionInfos infos;

    if (progress)
        progress->setTotal(datasets.count());

    for (auto& dataset : datasets) {
        if (progress)
            progress->step();

        auto lens = datasetLens(*dataset, datasets, norm_, true, true);

        Range rge = (gmaSlices > 0) ? lens->rgeGma() : lens->rgeGmaFull();
        if (rgeGma.isValid())
            rge = rge.intersect(rgeGma);

        if (rge.isEmpty())
            continue;

        gmaSlices = qMax(1u, gmaSlices);
        qreal step = rge.width() / gmaSlices;
        for_i (uint(gmaSlices)) {
            qreal min = rge.min + i * step;
            typ::Range gmaStripe(min, min + step);
            auto refInfo = makeReflectionInfo(*lens, reflection, gmaStripe);
            if (!qIsNaN(refInfo.inten()))
                infos.append(refInfo);
        }
    }

    return infos;
}

void Session::setIntenScaleAvg(bool avg, preal scale) {
    intenScaledAvg_ = avg;
    intenScale_ = scale;
}

void Session::addReflection(QString const& peakFunctionName) {
    calc::shp_Reflection reflection(new calc::Reflection(peakFunctionName));
    debug::ensure(!reflection.isNull());
    reflections_.append(reflection);
}

void Session::addReflection(const QJsonObject& obj) {
    calc::shp_Reflection reflection(new calc::Reflection);
    reflection->from_json(obj);
    reflections_.append(reflection);
}

qreal Session::calcAvgBackground(data::Dataset const& dataset) const {
    auto lens = datasetLens(dataset, dataset.datasets(), eNorm::NONE, true, true);
    Curve gmaCurve = lens->makeCurve(); // had argument averaged=true
    auto bgPolynom = Polynom::fromFit(bgPolyDegree_, gmaCurve, bgRanges_);
    return bgPolynom.avgY(lens->rgeTth());
}

qreal Session::calcAvgBackground(data::Datasets const& datasets) const {
    TakesLongTime __;
    qreal bg = 0;
    for (auto& dataset : datasets)
        bg += calcAvgBackground(*dataset);
    return bg / datasets.count();
}
