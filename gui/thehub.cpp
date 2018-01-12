// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/thehub.cpp
//! @brief     Implements class TheHub
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/thehub.h"
#include "core/loaders/loaders.h"
#include "core/session.h"
#include "core/typ/json.h"
#include "gui/models.h"
#include "gui/output/write_file.h"
#include "gui/widgets/new_q.h"
#include <QApplication>
#include <QDir>
#include <QJsonDocument>
#include <QStringBuilder> // for ".." % ..


TheHub::TheHub()
    : isFixedIntenImageScale_(false)
    , isFixedIntenDgramScale_(false)
    , isCombinedDgram_(false)
    , settings_("main_settings")
{
    qDebug() << "TheHub/";

    filesModel = new FilesModel();
    suiteModel = new DatasetsModel();
    metadataModel = new MetadataModel();
    reflectionsModel = new ReflectionsModel();

    connect(this, &TheHub::sigFilesChanged,
            [this]() { filesModel->signalReset(); });
    connect(this, &TheHub::sigSuitesChanged,
            [this]() { suiteModel->signalReset(); });
    connect(this, &TheHub::sigSuiteSelected,
            [this](shp_Suite dataseq) { metadataModel->reset(dataseq); });

    // create actions

    trigger_about = newQ::Trigger("About " + qApp->applicationName());
    trigger_online = newQ::Trigger("Open docs in external browser");
    trigger_checkUpdate = newQ::Trigger("Check for update");
    trigger_quit = newQ::Trigger("Quit");

    toggle_viewStatusbar = newQ::Toggle("Statusbar", true);
    toggle_viewFiles = newQ::Toggle("Files", true);
    toggle_viewDatasets = newQ::Toggle("Datasets", true);
    toggle_viewMetadata = newQ::Toggle("Metadata", true);
    trigger_viewReset = newQ::Trigger("Reset");
#ifndef Q_OS_OSX
    toggle_fullScreen = newQ::Toggle("FullScreen", false);
#endif

    trigger_loadSession = newQ::Trigger("Load session...");
    trigger_saveSession = newQ::Trigger("Save session...");
    trigger_clearSession = newQ::Trigger("Clear session");

    trigger_addFiles = newQ::Trigger("Add files...", ":/icon/add");
    trigger_removeFile = newQ::Trigger("Remove selected file(s)", ":/icon/rem");
    toggle_enableCorr = newQ::Toggle("Enable correction file...", false, ":/icon/useCorrection");
    trigger_remCorr = newQ::Trigger("Remove correction file", ":/icon/clear");

    trigger_rotateImage = newQ::Trigger("Rotate", ":/icon/rotate0");
    toggle_mirrorImage = newQ::Toggle("Mirror", false, ":/icon/mirrorHorz");
    toggle_linkCuts = newQ::Toggle("Link cuts", false, ":/icon/link");
    toggle_showOverlay = newQ::Toggle("Show overlay", false, ":/icon/crop");
    toggle_stepScale = newQ::Toggle("Scale in steps", false, ":/icon/steps");
    toggle_showBins = newQ::Toggle("Show bins", false, ":/icon/angle");

    toggle_fixedIntenImage = newQ::Toggle("Global intensity scale", false, ":/icon/scale");
    toggle_fixedIntenDgram = newQ::Toggle("Fixed intensity scale", false);

    toggle_combinedDgram = newQ::Toggle("All measurements", true);

    toggle_selRegions = newQ::Toggle("Select regions", false, ":/icon/selRegion");
    toggle_showBackground = newQ::Toggle("Show fitted background", false, ":/icon/showBackground");
    trigger_clearBackground = newQ::Trigger("Clear background regions", ":/icon/clear");
    trigger_clearReflections = newQ::Trigger("Clear reflections", ":/icon/clear");

    trigger_addReflection = newQ::Trigger("Add reflection", ":/icon/add");
    trigger_remReflection = newQ::Trigger("Remove reflection", ":/icon/rem");

    trigger_outputPolefigures = newQ::Trigger("Pole figures...");
    trigger_outputDiagrams = newQ::Trigger("Diagrams...");
    trigger_outputDiffractograms = newQ::Trigger("Diffractograms...");

    // key shortcuts

    trigger_quit->setShortcut(QKeySequence::Quit);

    toggle_viewStatusbar->setShortcut(Qt::Key_F12);
    toggle_viewFiles->setShortcut(Qt::Key_F8);
    toggle_viewDatasets->setShortcut(Qt::Key_F9);
    toggle_viewMetadata->setShortcut(Qt::Key_F10);

#ifndef Q_OS_OSX
    toggle_fullScreen->setShortcut(Qt::Key_F11);
#endif

    trigger_addFiles->setShortcut(Qt::CTRL | Qt::Key_O);
    trigger_removeFile->setShortcut(QKeySequence::Delete);
    toggle_enableCorr->setShortcut(Qt::SHIFT | Qt::CTRL | Qt::Key_C);

    trigger_rotateImage->setShortcut(Qt::CTRL | Qt::Key_R);

    // handle signals

    QObject::connect(this, &TheHub::sigFilesSelected,
                     [this](bool on) { trigger_removeFile->setEnabled(on); });
// TODO check                    !gSession->collectedFromFiles().isEmpty()); });
    QObject::connect(this, &TheHub::sigCorrFile,
            [this](shp_Datafile file) {
                         trigger_remCorr->setEnabled(!file.isNull()); });
    QObject::connect(this, &TheHub::sigCorrEnabled,
            [this](bool on) { toggle_enableCorr->setChecked(on); });

    auto deselect = [this]() {
        toggle_fixedIntenImage->setChecked(false);
        toggle_fixedIntenDgram->setChecked(false);
        toggle_combinedDgram->setChecked(false);
    };

    QObject::connect(this, &TheHub::sigGeometryChanged, [deselect]() { deselect(); });
    QObject::connect(this, &TheHub::sigSuitesChanged, [deselect]() { deselect(); });
    QObject::connect(this, &TheHub::sigCorrEnabled, [deselect]() { deselect(); });

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
        setImageRotate(gSession->imageTransform().nextRotate()); });

    saveDir = settings_.readStr("export_directory");
    saveFmt = settings_.readStr("export_format");

    qDebug() << "/TheHub";
}

TheHub::~TheHub() {
    settings_.saveStr("export_directory", saveDir);
    settings_.saveStr("export_format", saveFmt);
}

void TheHub::removeFile(uint i) {
    gSession->removeFile(i);
    emit sigFilesChanged();
    if (gSession->numFiles()==0)
        setImageCut(true, false, ImageCut());
}

void TheHub::saveSession(QFileInfo const& fileInfo) const {
    WriteFile file(fileInfo.filePath());

    QDir::setCurrent(fileInfo.absolutePath());
    const int result = file.write(saveSession());
    RUNTIME_CHECK(result >= 0, "Could not write session");
}

QByteArray TheHub::saveSession() const {

    QJsonObject top;

    const Geometry& geo = gSession->geometry();
    QJsonObject sub {
        { "distance", QJsonValue(geo.detectorDistance) },
        { "pixel size", QJsonValue(geo.pixSize) },
        { "beam offset", geo.midPixOffset.to_json() }
    };
    top.insert("detector", sub);

    const ImageCut& cut = gSession->imageCut();
    sub = {
        { "left", to_i(cut.left) },
        { "top", to_i(cut.top) },
        { "right", to_i(cut.right) },
        { "bottom", to_i(cut.bottom) } };
    top.insert("cut", sub);

    const ImageTransform& trn = gSession->imageTransform();
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
    top.insert("combine", to_i((uint)suiteGroupedBy_));

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

void TheHub::sessionFromFile(rcstr filePath) THROWS {
    QFile file(filePath);
    RUNTIME_CHECK(file.open(QIODevice::ReadOnly | QIODevice::Text),
                  "Cannot open file for reading: " % filePath);
    QDir::setCurrent(QFileInfo(filePath).absolutePath());
    sessionFromJson(file.readAll());
}

void TheHub::sessionFromJson(QByteArray const& json) THROWS {
    QJsonParseError parseError;
    QJsonDocument doc(QJsonDocument::fromJson(json, &parseError));
    RUNTIME_CHECK(QJsonParseError::NoError == parseError.error, "Error parsing session file");

    TakesLongTime __;

    gSession->clear();
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

    TR("sessionFromJson: going to collect suite");
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

    bool arg1 = top.loadBool("averaged intensity ", true);
    preal arg2 = top.loadPreal("intensity scale", preal(1));
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
            gSession->addGivenFile(io::loadDatafile(filePath));
        }
        emit sigFilesChanged();
    }
}

void TheHub::addGivenFiles(const QStringList& filePaths) THROWS {
    TakesLongTime __;
    for (rcstr filePath : filePaths)
        addGivenFile(filePath);
}

void TheHub::collectDatasetsFromFiles(uint_vec is, pint by) {
    gSession->collectDatasetsFromFiles((collectFromFiles_ = is), (suiteGroupedBy_ = by));
    emit sigFilesSelected(!gSession->collectedFromFiles().isEmpty());
    emit sigSuitesChanged();
}

void TheHub::collectDatasetsFromFiles(uint_vec is) {
    collectDatasetsFromFiles(is, suiteGroupedBy_);
}

void TheHub::combineDatasetsBy(pint by) {
    collectDatasetsFromFiles(collectFromFiles_, by);
}

void TheHub::setCorrFile(rcstr filePath) THROWS {
    shp_Datafile file;
    if (!filePath.isEmpty())
        file = io::loadDatafile(filePath);

    gSession->setCorrFile(file);
    emit sigCorrFile(file);

    tryEnableCorrection(true);
}

void TheHub::tryEnableCorrection(bool on) {
    gSession->tryEnableCorr(on);
    emit sigCorrEnabled(gSession->isCorrEnabled());
}

void TheHub::setImageCut(bool isTopOrLeft, bool linked, ImageCut const& cut) {
    gSession->setImageCut(isTopOrLeft, linked, cut);
    emit sigGeometryChanged();
}

void TheHub::setGeometry(preal detectorDistance, preal pixSize, IJ const& midPixOffset) {
    TR("setGeometry"); // keep an eye on this, since in the past circular calls may have happened

    gSession->setGeometry(detectorDistance, pixSize, midPixOffset);
    emit sigGeometryChanged();
}

void TheHub::setGammaRange(const Range& gammaRange) {
    gSession->setGammaRange(gammaRange);
    emit sigGammaRange();
}

void TheHub::setBgRanges(const Ranges& ranges) {
    gSession->setBgRanges(ranges);
    emit sigBgChanged();
}

void TheHub::addBgRange(const Range& range) {
    if (gSession->addBgRange(range))
        emit sigBgChanged();
}

void TheHub::removeBgRange(const Range& range) {
    if (gSession->removeBgRange(range))
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

void TheHub::setPeakFunction(const QString& peakFunctionName) {
    if (selectedReflection_) {
        selectedReflection_->setPeakFunction(peakFunctionName);
        emit sigReflectionsChanged();
    }
}

void TheHub::addReflection(const QString& peakFunctionName) {
    gSession->addReflection(peakFunctionName);
    emit sigReflectionsChanged();
}

void TheHub::remReflection(uint i) {
    gSession->remReflection(i);
    if (gSession->reflections().isEmpty())
        tellSelectedReflection(shp_Reflection());
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

void TheHub::tellSuiteSelected(shp_Suite suite) {
    selectedSuite_ = suite;
    emit sigSuiteSelected(suite);
}

void TheHub::tellSelectedReflection(shp_Reflection reflection) {
    selectedReflection_ = reflection;
    emit sigReflectionSelected(reflection);
}

void TheHub::tellReflectionData(shp_Reflection reflection) {
    emit sigReflectionData(reflection);
}

void TheHub::tellReflectionValues(
    const Range& rgeTth, qpair const& peak, fwhm_t fwhm, bool withGuesses) {
    emit sigReflectionValues(rgeTth, peak, fwhm, withGuesses);
}
