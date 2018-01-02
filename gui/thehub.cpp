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

#include "thehub.h"
#include "actions.h"
#include "data/datasequence.h"
#include "io/io_io.h"
#include "models.h"
#include "output/write_file.h"
#include "session.h"
#include "typ/json.h"
#include <QAction>
#include <QDir>
#include <QJsonDocument>
#include <QStringBuilder> // for ".." % ..

namespace gui {

TheHub::TheHub()
    : isFixedIntenImageScale_(false)
    , isFixedIntenDgramScale_(false)
    , isCombinedDgram_(false)
{
    qDebug() << "TheHub/";

    filesModel = new FilesModel();
    datasequenceModel = new DatasetsModel();
    metadataModel = new MetadataModel();
    reflectionsModel = new ReflectionsModel();

    connect(this, &gui::TheHubSignallingBase::sigFilesChanged,
            [this]() { filesModel->signalReset(); });
    connect(this, &gui::TheHubSignallingBase::sigDatasetsChanged,
            [this]() { datasequenceModel->signalReset(); });
    connect(this, &gui::TheHubSignallingBase::sigDatasetSelected,
            [this](QSharedPointer<DataSequence> dataset) { metadataModel->reset(dataset); });

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

    toggle_combinedDgram = newToggle("All datasequence");

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

    trigger_quit->setShortcut(QKeySequence::Quit);

    toggle_viewStatusbar->setShortcut(Qt::Key_F12);
    toggle_viewFiles->setShortcut(Qt::Key_F8);
    toggle_viewDatasets->setShortcut(Qt::Key_F9);
    toggle_viewDatasetInfo->setShortcut(Qt::Key_F10);

#ifndef Q_OS_OSX
    toggle_fullScreen->setShortcut(Qt::Key_F11);
#endif

    trigger_addFiles->setShortcut(Qt::CTRL | Qt::Key_O);
    trigger_removeFile->setShortcut(QKeySequence::Delete);
    toggle_enableCorr->setShortcut(Qt::SHIFT | Qt::CTRL | Qt::Key_C);

    trigger_rotateImage->setShortcut(Qt::CTRL | Qt::Key_R);

    // handle signals

    QObject::connect(this, &gui::TheHub::sigFilesSelected,
            [this]() { trigger_removeFile->setEnabled(
                    !gSession->collectedFromFiles().isEmpty()); });
    QObject::connect(this, &gui::TheHub::sigCorrFile,
            [this](QSharedPointer<Datafile const> file) { trigger_remCorr->setEnabled(!file.isNull()); });
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
    qDebug() << "/TheHub";
}

void TheHub::removeFile(uint i) {
    gSession->removeFile(i);
    emit sigFilesChanged();

    if (0 == gSession->numFiles())
        setImageCut(true, false, ImageCut());
}

QSharedPointer<calc::SequenceLens> TheHub::datasetLens(DataSequence const& dataset) const {
    return gSession->datasetLens(dataset, dataset.experiment(), gSession->norm(), true, true);
}

Curve TheHub::avgCurve(Experiment const& dss) const {
    return dss.avgCurve(*gSession);
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
        { "distance", QJsonValue(geo.detectorDistance) },
        { "pixel size", QJsonValue(geo.pixSize) },
        { "beam offset", geo.midPixOffset.to_json() }
    };
    top.insert("detector", sub);

    auto& cut = gSession->imageCut();
    sub = {
        { "left", to_i(cut.left) },
        { "top", to_i(cut.top) },
        { "right", to_i(cut.right) },
        { "bottom", to_i(cut.bottom) } };
    top.insert("cut", sub);

    auto& trn = gSession->imageTransform();
    top.insert("image transform", to_i((uint)trn.val));

    QJsonArray arrFiles;
    // save file path relative to location of session
    for_i (gSession->numFiles()) {
        str absPath = gSession->file(i)->fileInfo().absoluteFilePath();
        str relPath = QDir::current().relativeFilePath(absPath);
        arrFiles.append(relPath);
    }

    top.insert("files", arrFiles);

    QJsonArray arrSelectedFiles;
    for (uint i : gSession->collectedFromFiles())
        arrSelectedFiles.append(to_i(i));

    top.insert("selected files", arrSelectedFiles);
    top.insert("combine", to_i((uint)datasequenceGroupedBy_));

    if (gSession->hasCorrFile()) {
        str absPath = gSession->corrFile()->fileInfo().absoluteFilePath();
        str relPath = QDir::current().relativeFilePath(absPath);
        top.insert("correction file", relPath);
    }

    top.insert("background degree", to_i(gSession->bgPolyDegree()));
    top.insert("background ranges", gSession->bgRanges().to_json());
    top.insert("averaged intensity ", gSession->intenScaledAvg());
    top.insert("intensity scale", qreal_to_json((qreal)gSession->intenScale()));

    QJsonArray arrReflections;
    for (auto& reflection : gSession->reflections())
        arrReflections.append(reflection->to_json());

    top.insert("reflections", arrReflections);

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
    TR("sessionFromJson: cleared old session");

    JsonObj top(doc.object());

    const QJsonArray& files = top.loadArr("files");
    for (const QJsonValue& file : files) {
        str filePath = file.toString();
        QDir dir(filePath);
        RUNTIME_CHECK(dir.makeAbsolute(), str("Invalid file path: %1").arg(filePath));
        addGivenFile(dir.absolutePath());
    }

    const QJsonArray& sels = top.loadArr("selected files", true);
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

    TR("sessionFromJson: going to collect datasequence");
    collectDatasetsFromFiles(selIndexes, top.loadPint("combine", 1));

    TR("sessionFromJson: going to set correction file");
    setCorrFile(top.loadString("correction file", ""));

    TR("sessionFromJson: going to load detector geometry");
    const JsonObj& det = top.loadObj("detector");
    setGeometry(
        det.loadPreal("distance"), det.loadPreal("pixel size"),
        det.loadIJ("beam offset"));

    TR("sessionFromJson: going to load image cut");
    const JsonObj& cut = top.loadObj("cut");
    uint x1 = cut.loadUint("left"), y1 = cut.loadUint("top"),
         x2 = cut.loadUint("right"), y2 = cut.loadUint("bottom");
    setImageCut(true, false, ImageCut(x1, y1, x2, y2));
    setImageRotate(ImageTransform(top.loadUint("image transform")));

    TR("sessionFromJson: going to load fit setup");
    Ranges bgRanges;
    bgRanges.from_json(top.loadArr("background ranges"));
    setBgRanges(bgRanges);
    setBgPolyDegree(top.loadUint("background degree"));

    auto arg1 = top.loadBool("averaged intensity ", true);
    auto arg2 = top.loadPreal("intensity scale", preal(1));
    setIntenScaleAvg(arg1, arg2);

    TR("sessionFromJson: going to load reflections info");
    const QJsonArray& reflectionsInfo = top.loadArr("reflections");
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
    gSession->collectDatasetsFromFiles((collectFromFiles_ = is), (datasequenceGroupedBy_ = by));
    emit sigFilesSelected();
    emit sigDatasetsChanged();
}

void TheHub::collectDatasetsFromFiles(uint_vec is) {
    collectDatasetsFromFiles(is, datasequenceGroupedBy_);
}

void TheHub::combineDatasetsBy(pint by) {
    collectDatasetsFromFiles(collectFromFiles_, by);
}

Range TheHub::collectedDatasetsRgeGma() const {
    return gSession->collectedDatasets().rgeGma(*gSession);
}

void TheHub::setCorrFile(rcstr filePath) THROWS {
    QSharedPointer<Datafile const> file;
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

ImageCut const& TheHub::imageCut() const {
    return gSession->imageCut();
}

void TheHub::setImageCut(bool isTopOrLeft, bool linked, ImageCut const& cut) {
    gSession->setImageCut(isTopOrLeft, linked, cut);
    emit sigGeometryChanged();
}

void TheHub::setGeometry(preal detectorDistance, preal pixSize, IJ const& midPixOffset) {
    level_guard __(sigLevel_);
    if (sigLevel_ > 1)
        return;

    gSession->setGeometry(detectorDistance, pixSize, midPixOffset);
    emit sigGeometryChanged();
}

void TheHub::setGammaRange(Range const& gammaRange) {
    gSession->setGammaRange(gammaRange);
    emit sigGammaRange();
}

void TheHub::setBgRanges(Ranges const& ranges) {
    gSession->setBgRanges(ranges);
    emit sigBgChanged();
}

void TheHub::addBgRange(Range const& range) {
    if (gSession->addBgRange(range))
        emit sigBgChanged();
}

void TheHub::remBgRange(Range const& range) {
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

void TheHub::setImageRotate(ImageTransform rot) {
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
