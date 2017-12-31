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
#include "io/io_io.h"
#include "mainwin.h"
#include "output/write_file.h"
#include <QAction>
#include <QDir>
#include <QJsonDocument>
#include <QStringBuilder> // for ".." % ..

namespace gui {

TheHub::TheHub()
    : isFixedIntenImageScale_(false)
    , isFixedIntenDgramScale_(false)
    , isCombinedDgram_(false)
    , filesModel(*this)
    , datasetsModel(*this)
    , metadataModel(*this)
    , reflectionsModel(*this)
{
    using QKey = QKeySequence;

    // create actions

    trigger_about = newTrigger("About && Configuration...");
    trigger_online = newTrigger("Online documentation...");
    trigger_checkUpdate = newTrigger("Check for update...");
    trigger_quit = newTrigger("Quit");

    toggle_viewStatusbar = newToggle("Statusbar");
    toggle_viewFiles = newToggle("Files");
    toggle_viewDatasets = newToggle("Datasets");
    toggle_viewDatasetInfo = newToggle("Metadata");
    trigger_viewReset = newTrigger("Reset");
#ifndef Q_OS_OSX
    toggle_fullScreen = newToggle("FullScreen");
#endif

    trigger_loadSession = newTrigger("Load session...");
    trigger_saveSession = newTrigger("Save session...");
    trigger_clearSession = newTrigger("Clear session (to defaults)");

    trigger_addFiles = newTrigger("Add files...", ":/icon/add");
    trigger_removeFile = newTrigger("Remove selected file(s)", ":/icon/rem");
    toggle_enableCorr = newToggle("Enable correction file...", ":/icon/useCorrection");
    trigger_remCorr = newTrigger("Remove correction file", ":/icon/clear");

    trigger_rotateImage = newTrigger("Rotate", ":/icon/rotate0");
    toggle_mirrorImage = newToggle("Mirror", ":/icon/mirrorHorz");
    toggle_linkCuts = newToggle("Link cuts", ":/icon/link");
    toggle_showOverlay = newToggle("Show overlay", ":/icon/crop");
    toggle_stepScale = newToggle("Scale in steps", ":/icon/steps");
    toggle_showBins = newToggle("Show bins", ":/icon/angle");

    toggle_fixedIntenImage = newToggle("Global intensity scale", ":/icon/scale");
    toggle_fixedIntenDgram = newToggle("Fixed intensity scale");

    toggle_combinedDgram = newToggle("All datasets");

    toggle_selRegions = newToggle("Select regions", ":/icon/selRegion");
    toggle_showBackground = newToggle("Show fitted background", ":/icon/showBackground");
    trigger_clearBackground = newTrigger("Clear background regions", ":/icon/clear");
    trigger_clearReflections = newTrigger("Clear reflections", ":/icon/clear");

    trigger_addReflection = newTrigger("Add reflection", ":/icon/add");
    trigger_remReflection = newTrigger("Remove reflection", ":/icon/rem");

    trigger_outputPolefigures = newTrigger("Pole figures...");
    trigger_outputDiagrams = newTrigger("Diagrams...");
    trigger_outputDiffractograms = newTrigger("Diffractograms...");

    // key shortcuts

    trigger_quit->setShortcut(QKey::Quit);

    toggle_viewStatusbar->setShortcut(Qt::Key_F12);
    toggle_viewFiles->setShortcut(Qt::Key_F8);
    toggle_viewDatasets->setShortcut(Qt::Key_F9);
    toggle_viewDatasetInfo->setShortcut(Qt::Key_F10);

#ifndef Q_OS_OSX
    toggle_fullScreen->setShortcut(Qt::Key_F11);
#endif

    trigger_addFiles->setShortcut(Qt::CTRL | Qt::Key_O);
    trigger_removeFile->setShortcut(QKey::Delete);
    toggle_enableCorr->setShortcut(Qt::SHIFT | Qt::CTRL | Qt::Key_C);

    trigger_rotateImage->setShortcut(Qt::CTRL | Qt::Key_R);

    // handle signals

    QObject::connect(this, &gui::TheHub::sigFilesSelected,
            [this]() { trigger_removeFile->setEnabled(
                    !gSession->collectedFromFiles().isEmpty()); });
    QObject::connect(this, &gui::TheHub::sigCorrFile,
            [this](data::shp_File file) { trigger_remCorr->setEnabled(!file.isNull()); });
    QObject::connect(this, &gui::TheHub::sigCorrEnabled,
            [this](bool on) { toggle_enableCorr->setChecked(on); });

    auto deselect = [this]() {
        toggle_fixedIntenImage->setChecked(false);
        toggle_fixedIntenDgram->setChecked(false);
        toggle_combinedDgram->setChecked(false);
    };

    QObject::connect(this, &gui::TheHub::sigGeometryChanged,
                     [deselect]() { deselect(); });
    QObject::connect(this, &gui::TheHub::sigDatasetsChanged,
                     [deselect]() { deselect(); });
    QObject::connect(this, &gui::TheHub::sigCorrEnabled,
                     [deselect]() { deselect(); });

    trigger_removeFile->setEnabled(false);
    trigger_remReflection->setEnabled(false);

    connect(toggle_enableCorr, &QAction::toggled, [this](bool on) { tryEnableCorrection(on); });

    connect(trigger_remCorr, &QAction::triggered, [this]() { setCorrFile(""); });

    connect(toggle_fixedIntenImage, &QAction::toggled, [this](bool on) {
        isFixedIntenImageScale_ = on;
        emit sigDisplayChanged();
    });

    connect(toggle_fixedIntenDgram, &QAction::toggled, [this](bool on) {
        isFixedIntenDgramScale_ = on;
        emit sigDisplayChanged();
    });

    toggle_combinedDgram->setChecked(false);
    connect(toggle_combinedDgram, &QAction::toggled, [this](bool on) {
        isCombinedDgram_ = on;
        emit sigDisplayChanged();
    });

    connect(toggle_mirrorImage, &QAction::toggled, [this](bool on) { setImageMirror(on); });

    connect(trigger_rotateImage, &QAction::triggered, [this]() {
        setImageRotate(gSession->imageTransform().nextRotate());
    });
}

void TheHub::removeFile(uint i) {
    gSession->removeFile(i);
    emit sigFilesChanged();

    if (0 == gSession->numFiles())
        setImageCut(true, false, typ::ImageCut());
}

calc::shp_ImageLens TheHub::plainImageLens(typ::Image const& image) const {
    return gSession->imageLens(image, gSession->collectedDatasets(), true, false);
}

calc::shp_DatasetLens TheHub::datasetLens(data::Dataset const& dataset) const {
    return gSession->datasetLens(dataset, dataset.datasets(), gSession->norm(), true, true);
}

calc::ReflectionInfos TheHub::makeReflectionInfos(
    calc::Reflection const& reflection, uint gmaSlices, typ::Range const& rgeGma, Progress* progress) {
    return gSession->makeReflectionInfos(
        gSession->collectedDatasets(), reflection, gmaSlices, rgeGma, progress);
}

void TheHub::saveSession(QFileInfo const& fileInfo) const {
    WriteFile file(fileInfo.filePath());

    QDir::setCurrent(fileInfo.absolutePath());
    auto written = file.write(saveSession());
    RUNTIME_CHECK(written >= 0, "Could not write session");
}

QByteArray TheHub::saveSession() const {

    QJsonObject top;

    auto& geo = gSession->geometry();
    QJsonObject sub {
        { config_key::DET_DISTANCE, QJsonValue(geo.detectorDistance) },
        { config_key::DET_PIX_SIZE, QJsonValue(geo.pixSize) },
        { config_key::BEAM_OFFSET, geo.midPixOffset.to_json() }
    };
    top.insert(config_key::DETECTOR, sub);

    auto& cut = gSession->imageCut();
    sub = {
        { config_key::LEFT, to_i(cut.left) },
        { config_key::TOP, to_i(cut.top) },
        { config_key::RIGHT, to_i(cut.right) },
        { config_key::BOTTOM, to_i(cut.bottom) } };
    top.insert(config_key::CUT, sub);

    auto& trn = gSession->imageTransform();
    top.insert(config_key::TRANSFORM, to_i((uint)trn.val));

    QJsonArray arrFiles;
    // save file path relative to location of session
    for_i (gSession->numFiles()) {
        str absPath = gSession->file(i)->fileInfo().absoluteFilePath();
        str relPath = QDir::current().relativeFilePath(absPath);
        arrFiles.append(relPath);
    }

    top.insert(config_key::FILES, arrFiles);

    QJsonArray arrSelectedFiles;
    for (uint i : gSession->collectedFromFiles())
        arrSelectedFiles.append(to_i(i));

    top.insert(config_key::SELECTED_FILES, arrSelectedFiles);
    top.insert(config_key::COMBINE, to_i((uint)datasetsGroupedBy_));

    if (gSession->hasCorrFile()) {
        str absPath = gSession->corrFile()->fileInfo().absoluteFilePath();
        str relPath = QDir::current().relativeFilePath(absPath);
        top.insert(config_key::CORR_FILE, relPath);
    }

    top.insert(config_key::BG_DEGREE, to_i(gSession->bgPolyDegree()));
    top.insert(config_key::BG_RANGES, gSession->bgRanges().to_json());
    top.insert(config_key::INTEN_SCALED_AVG, gSession->intenScaledAvg());
    top.insert(config_key::INTEN_SCALE, qreal_to_json((qreal)gSession->intenScale()));

    QJsonArray arrReflections;
    for (auto& reflection : reflections())
        arrReflections.append(reflection->to_json());

    top.insert(config_key::REFLECTIONS, arrReflections);

    return QJsonDocument(top).toJson();
}

void TheHub::clearSession() {
    gSession->clear();
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

    const QJsonArray& reflectionsInfo = top.loadArr(config_key::REFLECTIONS);
    for_i (reflectionsInfo.count()) {
        gSession->addReflection(reflectionsInfo.at(i).toObject());
    }

    emit sigReflectionsChanged();
    TR("installed session from file");
}

void TheHub::addGivenFile(rcstr filePath) THROWS {
    if (!filePath.isEmpty() && !gSession->hasFile(filePath)) {
        {
            TakesLongTime __;
            gSession->addGivenFile(io::load(filePath));
        }
        emit sigFilesChanged();
    }
}

void TheHub::addGivenFiles(QStringList const& filePaths) THROWS {
    TakesLongTime __;
    for (auto& filePath : filePaths)
        addGivenFile(filePath);
}

void TheHub::collectDatasetsFromFiles(uint_vec is, pint by) {
    gSession->collectDatasetsFromFiles((collectFromFiles_ = is), (datasetsGroupedBy_ = by));
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
    return gSession->collectedDatasets().rgeGma(*gSession);
}

void TheHub::setCorrFile(rcstr filePath) THROWS {
    data::shp_File file;
    if (!filePath.isEmpty())
        file = io::load(filePath);

    gSession->setCorrFile(file);
    emit sigCorrFile(file);

    tryEnableCorrection(true);
}

void TheHub::tryEnableCorrection(bool on) {
    gSession->tryEnableCorr(on);
    emit sigCorrEnabled(gSession->isCorrEnabled());
}

typ::ImageCut const& TheHub::imageCut() const {
    return gSession->imageCut();
}

void TheHub::setImageCut(bool isTopOrLeft, bool linked, typ::ImageCut const& cut) {
    gSession->setImageCut(isTopOrLeft, linked, cut);
    emit sigGeometryChanged();
}

const typ::Geometry& TheHub::geometry() const {
    return gSession->geometry();
}

void TheHub::setGeometry(preal detectorDistance, preal pixSize, typ::IJ const& midPixOffset) {
    level_guard __(sigLevel_);
    if (sigLevel_ > 1)
        return;

    gSession->setGeometry(detectorDistance, pixSize, midPixOffset);
    emit sigGeometryChanged();
}

void TheHub::setGammaRange(typ::Range const& gammaRange) {
    gSession->setGammaRange(gammaRange);
    emit sigGammaRange();
}

void TheHub::setBgRanges(typ::Ranges const& ranges) {
    gSession->setBgRanges(ranges);
    emit sigBgChanged();
}

void TheHub::addBgRange(typ::Range const& range) {
    if (gSession->addBgRange(range))
        emit sigBgChanged();
}

void TheHub::remBgRange(typ::Range const& range) {
    if (gSession->remBgRange(range))
        emit sigBgChanged();
}

void TheHub::setBgPolyDegree(uint degree) {
    gSession->setBgPolyDegree(degree);
    emit sigBgChanged();
}

void TheHub::setIntenScaleAvg(bool avg, preal scale) {
    gSession->setIntenScaleAvg(avg, scale);
    emit sigNormChanged(); // TODO instead of another signal
}

void TheHub::setPeakFunction(QString const& peakFunctionName) {
    if (selectedReflection_) {
        selectedReflection_->setPeakFunction(peakFunctionName);
        emit sigReflectionsChanged();
    }
}

void TheHub::addReflection(QString const& peakFunctionName) {
    gSession->addReflection(peakFunctionName);
    emit sigReflectionsChanged();
}

void TheHub::remReflection(uint i) {
    gSession->remReflection(i);
    if (gSession->reflections().isEmpty())
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

    trigger_rotateImage->setIcon(QIcon(rotateIconFile));
    toggle_mirrorImage->setIcon(QIcon(mirrorIconFile));
    gSession->setImageTransformRotate(rot);
    setImageCut(true, false, gSession->imageCut());
    emit sigGeometryChanged();
}

void TheHub::setImageMirror(bool on) {
    toggle_mirrorImage->setChecked(on);
    gSession->setImageTransformMirror(on);
    emit sigGeometryChanged();
}

void TheHub::setNorm(eNorm norm) {
    gSession->setNorm(norm);
    emit sigNormChanged();
}

} // namespace gui
