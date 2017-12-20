// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/thehub.cpp
//! @brief     Implements class TheHub
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "config.h"
#include "def/idiomatic_for.h"
#include "io/io_io.h"
#include "mainwin.h"
#include "output/write_file.h"
#include "typ/exception.h"
#include "typ/range.h"
#include "typ/str.h"
#include <QDir>
#include <QJsonDocument>
#include <QMessageBox>
#include <QSpinBox>

namespace gui {

TheHub::TheHub()
    : actions(*this)
    , session_(new core::Session())
    , isFixedIntenImageScale_(false)
    , isFixedIntenDgramScale_(false)
    , isCombinedDgram_(false)
    , filesModel(*this)
    , datasetsModel(*this)
    , metadataModel(*this)
    , reflectionsModel(*this)
{
    configActions();
}

void TheHub::configActions() {
    actions.remFile->setEnabled(false);
    actions.remReflection->setEnabled(false);

    connect(actions.enableCorr, &QAction::toggled, [this](bool on) { tryEnableCorrection(on); });

    connect(actions.remCorr, &QAction::triggered, [this]() { setCorrFile(""); });

    connect(actions.fixedIntenImage, &QAction::toggled, [this](bool on) {
        isFixedIntenImageScale_ = on;
        emit sigDisplayChanged();
    });

    connect(actions.fixedIntenDgram, &QAction::toggled, [this](bool on) {
        isFixedIntenDgramScale_ = on;
        emit sigDisplayChanged();
    });

    actions.combinedDgram->setChecked(false);
    connect(actions.combinedDgram, &QAction::toggled, [this](bool on) {
        isCombinedDgram_ = on;
        emit sigDisplayChanged();
    });

    connect(actions.mirrorImage, &QAction::toggled, [this](bool on) { setImageMirror(on); });

    connect(actions.rotateImage, &QAction::triggered, [this]() {
        setImageRotate(session_->imageTransform().nextRotate());
    });
}

void TheHub::remFile(uint i) {
    session_->remFile(i);
    emit sigFilesChanged();

    if (0 == numFiles())
        setImageCut(true, false, typ::ImageCut());
}

bool TheHub::hasCorrFile() const { return session_->hasCorrFile(); }

calc::shp_ImageLens TheHub::plainImageLens(typ::Image const& image) const {
    return session_->imageLens(image, collectedDatasets(), true, false);
}

calc::shp_DatasetLens TheHub::datasetLens(data::Dataset const& dataset) const {
    return session_->datasetLens(dataset, dataset.datasets(), session_->norm(), true, true);
}

calc::ReflectionInfos TheHub::makeReflectionInfos(
    calc::Reflection const& reflection, uint gmaSlices, typ::Range const& rgeGma, Progress* progress) {
    return session_->makeReflectionInfos(
        collectedDatasets(), reflection, gmaSlices, rgeGma, progress);
}

void TheHub::saveSession(QFileInfo const& fileInfo) const {
    WriteFile file(fileInfo.filePath());

    QDir::setCurrent(fileInfo.absolutePath());
    auto written = file.write(saveSession());
    RUNTIME_CHECK(written >= 0, "Could not write session");
}

QByteArray TheHub::saveSession() const {
    using typ::JsonObj;

    JsonObj top;

    auto& geo = session_->geometry();
    QJsonObject sub {
        { config_key::DET_DISTANCE, QJsonValue(geo.detectorDistance) },
        { config_key::DET_PIX_SIZE, QJsonValue(geo.pixSize) },
        { config_key::BEAM_OFFSET, geo.midPixOffset.to_json() }
    };
    top.insert(config_key::DETECTOR, sub);

    auto& cut = session_->imageCut();
    sub = {
        { config_key::LEFT, to_i(cut.left) },
        { config_key::TOP, to_i(cut.top) },
        { config_key::RIGHT, to_i(cut.right) },
        { config_key::BOTTOM, to_i(cut.bottom) } };
    top.insert(config_key::CUT, sub);

    auto& trn = session_->imageTransform();
    top.insert(config_key::TRANSFORM, to_i((uint)trn.val));

    QJsonArray arrFiles;
    // save file path relative to location of session
    for_i (numFiles()) {
        str absPath = getFile(i)->fileInfo().absoluteFilePath();
        str relPath = QDir::current().relativeFilePath(absPath);
        arrFiles.append(relPath);
    }

    top.insert(config_key::FILES, arrFiles);

    QJsonArray arrSelectedFiles;
    for (uint i : collectedFromFiles())
        arrSelectedFiles.append(to_i(i));

    top.insert(config_key::SELECTED_FILES, arrSelectedFiles);
    top.insert(config_key::COMBINE, to_i((uint)datasetsGroupedBy_));

    if (hasCorrFile()) {
        str absPath = session_->corrFile()->fileInfo().absoluteFilePath();
        str relPath = QDir::current().relativeFilePath(absPath);
        top.insert(config_key::CORR_FILE, relPath);
    }

    top.insert(config_key::BG_DEGREE, to_i(bgPolyDegree()));
    top.insert(config_key::BG_RANGES, bgRanges().to_json());
    top.insert(config_key::INTEN_SCALED_AVG, intenScaledAvg());
    top.saveQreal(config_key::INTEN_SCALE, (qreal)intenScale());

    QJsonArray arrReflections;
    for (auto& reflection : reflections())
        arrReflections.append(reflection->to_json().sup());

    top.insert(config_key::REFLECTIONS, arrReflections);

    return QJsonDocument(top.sup()).toJson();
}

void TheHub::clearSession() {
    session_->clear();
    tellSessionCleared();
}

void TheHub::sessionFromFile(QFileInfo const& fileInfo) THROWS {
    QFile file(fileInfo.absoluteFilePath());
    RUNTIME_CHECK(file.open(QIODevice::ReadOnly | QIODevice::Text),
                  "Cannot open file for reading: " % fileInfo.absoluteFilePath());
    QDir::setCurrent(fileInfo.absolutePath());
    sessionFromJson(file.readAll());
}

void TheHub::sessionFromJson(QByteArray const& json) THROWS {
    QJsonParseError parseError;
    QJsonDocument doc(QJsonDocument::fromJson(json, &parseError));
    RUNTIME_CHECK(QJsonParseError::NoError == parseError.error, "Error parsing session file");

    TakesLongTime __;

    clearSession();
    TR("cleared old session");

    typ::JsonObj top(doc.object());

    const QJsonArray& files = top.loadArr(config_key::FILES);
    for (const QJsonValue& file : files) {
        str filePath = file.toString();
        QDir dir(filePath);
        RUNTIME_CHECK(dir.makeAbsolute(), str("Invalid file path: %1").arg(filePath));
        addGivenFile(dir.absolutePath());
    }

    const QJsonArray& sels = top.loadArr(config_key::SELECTED_FILES, true);
    uint_vec selIndexes;
    for (const QJsonValue& sel : sels) {
        int i = sel.toInt(), index = qBound(0, i, files.count());
        RUNTIME_CHECK(i == index, str("Invalid selection index: %1").arg(i));
        selIndexes.append(to_u(index));
    }

    std::sort(selIndexes.begin(), selIndexes.end());
    int lastIndex = -1;
    for (uint index : selIndexes) {
        RUNTIME_CHECK(lastIndex < to_i(index), str("Duplicate selection index"));
        lastIndex = to_i(index);
    }

    collectDatasetsFromFiles(selIndexes, top.loadPint(config_key::COMBINE, 1));

    setCorrFile(top.loadString(config_key::CORR_FILE, ""));

    const typ::JsonObj& det = top.loadObj(config_key::DETECTOR);
    setGeometry(
        det.loadPreal(config_key::DET_DISTANCE), det.loadPreal(config_key::DET_PIX_SIZE),
        det.loadIJ(config_key::BEAM_OFFSET));

    const typ::JsonObj& cut = top.loadObj(config_key::CUT);
    uint x1 = cut.loadUint(config_key::LEFT), y1 = cut.loadUint(config_key::TOP),
         x2 = cut.loadUint(config_key::RIGHT), y2 = cut.loadUint(config_key::BOTTOM);
    setImageCut(true, false, typ::ImageCut(x1, y1, x2, y2));

    setImageRotate(typ::ImageTransform(top.loadUint(config_key::TRANSFORM)));

    typ::Ranges bgRanges;
    bgRanges.from_json(top.loadArr(config_key::BG_RANGES));
    setBgRanges(bgRanges);

    setBgPolyDegree(top.loadUint(config_key::BG_DEGREE));

    setIntenScaleAvg(
        top.loadBool(config_key::INTEN_SCALED_AVG, true),
        top.loadPreal(config_key::INTEN_SCALE, preal(1)));

    const QJsonArray& reflectionsObj = top.loadArr(config_key::REFLECTIONS);
    for_i (reflectionsObj.count()) {
        calc::shp_Reflection reflection(new calc::Reflection);
        reflection->from_json(reflectionsObj.at(i).toObject());
        session_->addReflection(reflection);
    }

    emit sigReflectionsChanged();
    TR("installed session from file");
}

void TheHub::addGivenFile(rcstr filePath) THROWS {
    if (!filePath.isEmpty() && !session_->hasFile(filePath)) {
        {
            TakesLongTime __;
            session_->addGivenFile(io::load(filePath));
        }
        emit sigFilesChanged();
    }
}

void TheHub::addGivenFiles(str_lst const& filePaths) THROWS {
    TakesLongTime __;
    for (auto& filePath : filePaths)
        addGivenFile(filePath);
}

void TheHub::collectDatasetsFromFiles(uint_vec is, pint by) {
    session_->collectDatasetsFromFiles((collectFromFiles_ = is), (datasetsGroupedBy_ = by));
    emit sigFilesSelected();
    emit sigDatasetsChanged();
}

void TheHub::collectDatasetsFromFiles(uint_vec is) {
    collectDatasetsFromFiles(is, datasetsGroupedBy_);
}

void TheHub::combineDatasetsBy(pint by) {
    collectDatasetsFromFiles(collectFromFiles_, by);
}

typ::Range TheHub::collectedDatasetsRgeGma() const {
    return collectedDatasets().rgeGma(*session_);
}

void TheHub::setCorrFile(rcstr filePath) THROWS {
    data::shp_File file;
    if (!filePath.isEmpty())
        file = io::load(filePath);

    session_->setCorrFile(file);
    emit sigCorrFile(file);

    tryEnableCorrection(true);
}

void TheHub::tryEnableCorrection(bool on) {
    session_->tryEnableCorr(on);
    emit sigCorrEnabled(session_->isCorrEnabled());
}

typ::ImageCut const& TheHub::imageCut() const {
    return session_->imageCut();
}

void TheHub::setImageCut(bool isTopOrLeft, bool linked, typ::ImageCut const& cut) {
    session_->setImageCut(isTopOrLeft, linked, cut);
    emit sigGeometryChanged();
}

const typ::Geometry& TheHub::geometry() const {
    return session_->geometry();
}

void TheHub::setGeometry(preal detectorDistance, preal pixSize, typ::IJ const& midPixOffset) {
    level_guard __(sigLevel_);
    if (sigLevel_ > 1)
        return;

    session_->setGeometry(detectorDistance, pixSize, midPixOffset);
    emit sigGeometryChanged();
}

void TheHub::setGammaRange(typ::Range const& gammaRange) {
    session_->setGammaRange(gammaRange);
    emit sigGammaRange();
}

void TheHub::setBgRanges(typ::Ranges const& ranges) {
    session_->setBgRanges(ranges);
    emit sigBgChanged();
}

void TheHub::addBgRange(typ::Range const& range) {
    if (session_->addBgRange(range))
        emit sigBgChanged();
}

void TheHub::remBgRange(typ::Range const& range) {
    if (session_->remBgRange(range))
        emit sigBgChanged();
}

void TheHub::setBgPolyDegree(uint degree) {
    session_->setBgPolyDegree(degree);
    emit sigBgChanged();
}

void TheHub::setIntenScaleAvg(bool avg, preal scale) {
    session_->setIntenScaleAvg(avg, scale);
    emit sigNormChanged(); // TODO instead of another signal
}

void TheHub::setReflType(fit::ePeakType type) {
    if (selectedReflection_) {
        selectedReflection_->setType(type);
        emit sigReflectionsChanged();
    }
}

void TheHub::addReflection(fit::ePeakType type) {
    session_->addReflection(calc::shp_Reflection(new calc::Reflection(type)));
    emit sigReflectionsChanged();
}

void TheHub::remReflection(uint i) {
    session_->remReflection(i);
    if (session_->reflections().isEmpty())
        tellSelectedReflection(calc::shp_Reflection());

    emit sigReflectionsChanged();
}

void TheHub::setFittingTab(eFittingTab tab) {
    emit sigFittingTab((fittingTab_ = tab));
}

void TheHub::setImageRotate(typ::ImageTransform rot) {
    const char* rotateIconFile;
    const char* mirrorIconFile;

    switch (rot.val & 3) {
    default /* case */:
        rotateIconFile = ":/icon/rotate0";
        mirrorIconFile = ":/icon/mirrorHorz";
        break;
    case 1:
        rotateIconFile = ":/icon/rotate1";
        mirrorIconFile = ":/icon/mirrorVert";
        break;
    case 2:
        rotateIconFile = ":/icon/rotate2";
        mirrorIconFile = ":/icon/mirrorHorz";
        break;
    case 3:
        rotateIconFile = ":/icon/rotate3";
        mirrorIconFile = ":/icon/mirrorVert";
        break;
    }

    actions.rotateImage->setIcon(QIcon(rotateIconFile));
    actions.mirrorImage->setIcon(QIcon(mirrorIconFile));
    session_->setImageTransformRotate(rot);
    setImageCut(true, false, session_->imageCut());
    emit sigGeometryChanged();
}

void TheHub::setImageMirror(bool on) {
    actions.mirrorImage->setChecked(on);
    session_->setImageTransformMirror(on);
    emit sigGeometryChanged();
}

void TheHub::setNorm(eNorm norm) {
    session_->setNorm(norm);
    emit sigNormChanged();
}

} // namespace gui
