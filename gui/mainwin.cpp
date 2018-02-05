// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/mainwin.cpp
//! @brief     Implements class MainWin
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "mainwin.h"
#include "../manifest.h"
#include "core/session.h"
#include "gui/base/new_q.h"
#include "gui/console.h"
#include "gui/output/output_diagrams.h"
#include "gui/output/output_diffractograms.h"
#include "gui/output/output_polefigures.h"
#include "gui/panels/subframe_diffractogram.h"
#include "gui/panels/subframe_files.h"
#include "gui/panels/subframe_image.h"
#include "gui/panels/subframe_measurements.h"
#include "gui/panels/subframe_metadata.h"
#include "gui/panels/subframe_setup.h"
#include "gui/popup/about.h"
#include "gui/popup/filedialog.h"
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDir>
#include <QJsonDocument>
#include <QMenuBar>
#include <QMessageBox>
#include <QNetworkReply>
#include <QSplitter>
#include <QStatusBar>
#include <QStringBuilder> // for ".." % ..

MainWin* gMainWin; //!< global, for message handling
MainWin* gHub; //!< global, for signalling and command flow

// ************************************************************************** //
//  file-scoped functions
// ************************************************************************** //

namespace {

//! Initialize the menu bar. Part of the MainWin initialization.
void initMenus(QMenuBar* mbar) {

    auto _separator = [mbar]()->QAction* {
        QAction* ret = new QAction(mbar);
        ret->setSeparator(true);
        return ret;
    };

    auto _actionsToMenu = [mbar](const char* menuName, QList<QAction*> actions)->QMenu* {
        QMenu* menu = mbar->addMenu(menuName);
        menu->addActions(actions);
        str prefix = str("%1: ").arg(menu->title().remove('&'));
        for (auto action : actions)
            action->setToolTip(prefix + action->toolTip());
        return menu;
    };

#ifdef Q_OS_OSX
    mbar->setNativeMenuBar(false); // REVIEW
#else
    mbar->setNativeMenuBar(true);
#endif

    _actionsToMenu(
        "&File",
        {
            gHub->trigger_addFiles,
                gHub->trigger_removeFile,
                _separator(),
                gHub->trigger_corrFile,
                gHub->toggle_enableCorr,
                _separator(),
                gHub->trigger_loadSession,
                gHub->trigger_saveSession,
                gHub->trigger_clearSession,
#ifndef Q_OS_OSX // Mac puts Quit into the Apple menu
                _separator(),
#endif
                gHub->trigger_quit,
        });

    QMenu* menuImage = _actionsToMenu(
        "&Image",
        {   gHub->trigger_rotateImage,
                gHub->toggle_mirrorImage,
                gHub->toggle_fixedIntenImage,
                gHub->toggle_linkCuts,
                gHub->toggle_showOverlay,
                gHub->toggle_stepScale,
                gHub->toggle_showBins,
        });
    menuImage->setEnabled(false);
    QObject::connect(gSession, &Session::sigFiles, [menuImage]()
                     { menuImage->setEnabled(gSession->dataset().countFiles()); });

    QMenu* menuDgram = _actionsToMenu(
        "&Diffractogram",
        {
            gHub->toggle_selRegions,
                gHub->toggle_showBackground,
                gHub->trigger_clearBackground,
                gHub->trigger_clearPeaks,
                _separator(),
                gHub->trigger_addPeak,
                gHub->trigger_removePeak,
                _separator(),
                gHub->toggle_combinedDgram,
                gHub->toggle_fixedIntenDgram,
        });
    menuDgram->setEnabled(false);
    QObject::connect(gSession, &Session::sigFiles, [menuDgram]()
                     { menuDgram->setEnabled(gSession->dataset().countFiles()); });

    QMenu* menuOutput = _actionsToMenu(
        "&Output",
        {
            gHub->trigger_outputPolefigures,
                gHub->trigger_outputDiagrams,
                gHub->trigger_outputDiffractograms,
        });
    menuOutput->setEnabled(false);
    QObject::connect(gSession, &Session::sigActivated, [menuOutput]()
                     { menuOutput->setEnabled(gSession->experiment().size()); });

    _actionsToMenu(
        "&View",
        {   gHub->toggle_viewFiles,
                gHub->toggle_viewDatasets,
                gHub->toggle_viewMetadata,
                _separator(),
#ifndef Q_OS_OSX
                gHub->toggle_fullScreen,
#endif
                gHub->toggle_viewStatusbar,
                _separator(),
                gHub->trigger_viewReset,
        });

    _actionsToMenu(
        "&Help",
        {
            gHub->trigger_about, // Mac puts About into the Apple menu
                gHub->trigger_online,
                gHub->trigger_checkUpdate,
        });
}

} // local methods

// ************************************************************************** //
//  class MainWin
// ************************************************************************** //

MainWin::MainWin()
    : isFixedIntenImageScale_(false)
    , isFixedIntenDgramScale_(false)
    , isCombinedDgram_(false)
    , settings_("main_settings")
{
    qDebug() << "MainWin/";
    setWindowIcon(QIcon(":/icon/retroStier"));
    QDir::setCurrent(QDir::homePath());
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);

    initMenus(menuBar());
    initLayout();
    connectActions();
    readSettings();
    QObject::connect(gConsole, &Console::transmitLine, this, &MainWin::execCommand);

    // create actions

    trigger_about = newQ::Trigger("trigger_about", "About " + qApp->applicationName());

    trigger_online = newQ::Trigger("trigger_online", "Open docs in external browser");

    trigger_checkUpdate = newQ::Trigger("trigger_checkUpdate", "Check for update");

    trigger_quit = newQ::Trigger("trigger_quit", "Quit");
    trigger_quit->setShortcut(QKeySequence::Quit);

    toggle_viewStatusbar = newQ::Toggle("toggle_viewStatusbar", "Statusbar", true);
    toggle_viewStatusbar->setShortcut(Qt::Key_F12);

    toggle_viewFiles = newQ::Toggle("toggle_viewFiles", "Files", true);
    toggle_viewFiles->setShortcut(Qt::Key_F8);

    toggle_viewDatasets = newQ::Toggle("toggle_viewDatasets", "Datasets", true);
    toggle_viewDatasets->setShortcut(Qt::Key_F9);

    toggle_viewMetadata = newQ::Toggle("toggle_viewMetadata", "Metadata", true);
    toggle_viewMetadata->setShortcut(Qt::Key_F10);

    trigger_viewReset = newQ::Trigger("trigger_viewReset", "Reset");

#ifndef Q_OS_OSX
    toggle_fullScreen = newQ::Toggle("toggle_fullScreen", "FullScreen", false);
    toggle_fullScreen->setShortcut(Qt::Key_F11);
#endif

    trigger_loadSession = newQ::Trigger("trigger_loadSession", "Load session...");

    trigger_saveSession = newQ::Trigger("trigger_saveSession", "Save session...");

    trigger_clearSession = newQ::Trigger("trigger_clearSession", "Clear session");

    trigger_addFiles = newQ::Trigger("trigger_addFiles", "Add files...", ":/icon/add");
    trigger_addFiles->setShortcut(Qt::CTRL | Qt::Key_O);

    trigger_removeFile = newQ::Trigger("trigger_removeFile", "Remove highlighted file", ":/icon/rem");
    trigger_removeFile->setShortcut(QKeySequence::Delete);
    trigger_removeFile->setEnabled(false);
    QObject::connect(trigger_removeFile, &QAction::triggered, []() {
            gSession->dataset().removeFile(); });
    QObject::connect(gSession, &Session::sigFiles, [this]() {
            trigger_removeFile->setEnabled(gSession->dataset().countFiles()); });

    trigger_corrFile = newQ::Trigger("trigger_corrFile", "Add correction file", ":/icon/add");
    trigger_corrFile->setShortcut(Qt::SHIFT | Qt::CTRL | Qt::Key_O);
    connect(trigger_corrFile, &QAction::triggered, this, &MainWin::loadCorrFile);
    QObject::connect(gSession, &Session::sigCorr, [this]() {
            bool hasFile = gSession->hasCorrFile();
            qDebug() << "on sigCorr " << hasFile;
            trigger_corrFile->setIcon(QIcon(hasFile ? ":/icon/rem" : ":/icon/add"));
            QString text = QString(hasFile ? "Remove" : "Add") + " correction file";
            trigger_corrFile->setText(text);
            trigger_corrFile->setToolTip(text.toLower());
        });

    toggle_enableCorr = newQ::Toggle("toggle_enableCorr", "Enable correction file", false, ":/icon/useCorrection");
    connect(toggle_enableCorr, &QAction::toggled, [this](bool on) {
            gSession->corrset().tryEnable(on); });
    QObject::connect(gSession, &Session::sigCorr, [this]() {
            toggle_enableCorr->setEnabled(gSession->hasCorrFile());
            toggle_enableCorr->setChecked(gSession->corrset().isEnabled());
        });

    trigger_rotateImage = newQ::Trigger("trigger_rotateImage", "Rotate", ":/icon/rotate0");
    trigger_rotateImage->setShortcut(Qt::CTRL | Qt::Key_R);
    connect(trigger_rotateImage, &QAction::triggered, [this]() {
        setImageRotate(gSession->imageTransform().nextRotate()); });

    toggle_mirrorImage = newQ::Toggle("toggle_mirrorImage", "Mirror", false, ":/icon/mirrorHorz");
    connect(toggle_mirrorImage, &QAction::toggled, [this](bool on) { setImageMirror(on); });

    toggle_linkCuts = newQ::Toggle("toggle_linkCuts", "Link cuts", false, ":/icon/link");

    toggle_showOverlay = newQ::Toggle("toggle_showOverlay", "Show overlay", false, ":/icon/crop");

    toggle_stepScale = newQ::Toggle("toggle_stepScale", "Scale in steps", false, ":/icon/steps");

    toggle_showBins = newQ::Toggle("toggle_showBins", "Show bins", false, ":/icon/angle");

    toggle_fixedIntenImage = newQ::Toggle("toggle_fixedIntenImage", "Global intensity scale", false, ":/icon/scale");
    connect(toggle_fixedIntenImage, &QAction::toggled, [this](bool on) {
        isFixedIntenImageScale_ = on;
        emit sigDisplayChanged(); });

    toggle_fixedIntenDgram = newQ::Toggle("toggle_fixedIntenDgram", "Fixed intensity scale", false);
    connect(toggle_fixedIntenDgram, &QAction::toggled, [this](bool on) {
        isFixedIntenDgramScale_ = on;
        emit sigDisplayChanged(); });

    toggle_combinedDgram = newQ::Toggle("toggle_combinedDgram", "All measurements", true);
    toggle_combinedDgram->setChecked(false);
    connect(toggle_combinedDgram, &QAction::toggled, [this](bool on) {
        isCombinedDgram_ = on;
        emit sigDisplayChanged(); });

    toggle_selRegions = newQ::Toggle("toggle_selRegions", "Select regions", false, ":/icon/selRegion");

    toggle_showBackground = newQ::Toggle("toggle_showBackground", "Show fitted background", false, ":/icon/showBackground");

    trigger_clearBackground = newQ::Trigger("trigger_clearBackground", "Clear background regions", ":/icon/clear");
    connect(trigger_clearBackground, &QAction::triggered, [this]() {
            gSession->baseline().setRanges({}); });

    trigger_clearPeaks = newQ::Trigger("trigger_clearPeaks", "Clear peaks", ":/icon/clear");

    trigger_addPeak = newQ::Trigger("trigger_addPeak", "Add peak", ":/icon/add");

    trigger_removePeak = newQ::Trigger("trigger_removePeak", "Remove peak", ":/icon/rem");
    trigger_removePeak->setEnabled(false);
    QObject::connect(gSession, &Session::sigPeaks, [this]() {
            trigger_removePeak->setEnabled(gSession->peaks().count()); });

    trigger_outputPolefigures = newQ::Trigger("trigger_outputPolefigures", "Pole figures...");
    QObject::connect(gSession, &Session::sigPeaks, [this]()
                     { trigger_outputPolefigures->setEnabled(gSession->peaks().count()); });

    trigger_outputDiagrams = newQ::Trigger("trigger_outputDiagrams", "Diagrams...");

    trigger_outputDiffractograms = newQ::Trigger("trigger_outputDiffractograms", "Diffractograms...");

    saveDir = settings_.readStr("export_directory");
    saveFmt = settings_.readStr("export_format");

    qDebug() << "/MainWin";
}

MainWin::~MainWin() {
    settings_.saveStr("export_directory", saveDir);
    settings_.saveStr("export_format", saveFmt);
}

void MainWin::initLayout() {
    addDockWidget(Qt::LeftDockWidgetArea, (dockFiles_ = new SubframeFiles()));
    addDockWidget(Qt::LeftDockWidgetArea, (dockMeasurements_ = new SubframeMeasurements()));
    addDockWidget(Qt::LeftDockWidgetArea, (dockDatasetInfo_ = new SubframeMetadata()));

    auto splMain = new QSplitter(Qt::Vertical);
    splMain->setChildrenCollapsible(false);

    auto splTop = new QSplitter(Qt::Horizontal);
    splTop->setChildrenCollapsible(false);

    setCentralWidget(splMain);

    splMain->addWidget(splTop);
    splMain->addWidget(new SubframeDiffractogram());
    splMain->setStretchFactor(1, 1);

    splTop->addWidget(new SubframeSetup());
    splTop->addWidget(new SubframeImage());
    splTop->setStretchFactor(1, 1);

    statusBar();
}

//! Connect signals to slots. Part of the MainWin initialization.
void MainWin::connectActions() {

#define connectTrigger(action, fun) QObject::connect(action, &QAction::triggered, this, fun)
#define connectToggle(action, fun)  QObject::connect(action, &QAction::toggled, this, fun)

    connectTrigger(trigger_loadSession, &MainWin::loadSession);
    connectTrigger(trigger_saveSession, &MainWin::saveSession);
    QObject::connect(trigger_clearSession, &QAction::triggered, gSession,  &Session::clear);

    connectTrigger(trigger_addFiles, &MainWin::addFiles);

    connectTrigger(trigger_quit, &QMainWindow::close);

    QObject::connect(trigger_outputPolefigures, &QAction::triggered,
                     [this](){PoleFiguresFrame("Pole Figures", this).exec();});
    QObject::connect(trigger_outputDiagrams, &QAction::triggered,
                     [this](){DiagramsFrame("Diagrams", this).exec();});
    QObject::connect(trigger_outputDiffractograms, &QAction::triggered,
                     [this](){DiffractogramsFrame("Diffractograms", this).exec();});

    QObject::connect(trigger_about, &QAction::triggered, [this](){AboutBox(this).exec();});
    connectTrigger(trigger_online, &MainWin::online);
    connectTrigger(trigger_checkUpdate, &MainWin::checkUpdate);

    connectToggle(toggle_viewStatusbar, &MainWin::viewStatusbar);
#ifndef Q_OS_OSX
    connectToggle(toggle_fullScreen, &MainWin::viewFullScreen);
#endif

    connectToggle(toggle_viewFiles, &MainWin::viewFiles);
    connectToggle(toggle_viewDatasets, &MainWin::viewDatasets);
    connectToggle(toggle_viewMetadata, &MainWin::viewMetadata);

    connectTrigger(trigger_viewReset, &MainWin::viewReset);
}

void MainWin::online() {
    QDesktopServices::openUrl(QUrl(STECA2_PAGES_URL));
}

void MainWin::checkUpdate() {

    QNetworkRequest req;

    str ver = qApp->applicationVersion();
    str qry = ver % "\t| " % QSysInfo::prettyProductName();
    req.setUrl(QUrl(str(STECA2_VERSION_URL) % "?" % qry));
    QNetworkReply* reply = netMan_.get(req);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (QNetworkReply::NoError != reply->error()) {
            qWarning() << "Network Error: " << reply->errorString();
            return;
        }
        str ver = qApp->applicationVersion();
        str lastVer = reply->readAll().trimmed();
        str name = qApp->applicationName();
        str result;
        if (ver != lastVer)
            result = str(
                "<p>The latest released %1 version is %2. You have "
                "version %3.</p>"
                "<p><a href='%4'>Open download location in external browser</a></p>")
                .arg(name, lastVer, ver, STECA2_DOWNLOAD_URL);
        else
            result = str(
                "<p>You have the latest released %1 version (%2).</p>").arg(name).arg(ver);
        QMessageBox::information(this, str("%1 update").arg(name), result);
        });
}

void MainWin::addFiles() {
    QStringList fileNames = file_dialog::openFileNames(
        this, "Add files", QDir::current().absolutePath(),
        "Data files (*.dat *.mar*);;All files (*.*)");
    repaint();
    if (fileNames.isEmpty())
        return;
    QDir::setCurrent(QFileInfo(fileNames.at(0)).absolutePath());
    qDebug() << "going to load " << fileNames;
    TakesLongTime __;
    gSession->dataset().addGivenFiles(fileNames);
}

void MainWin::loadSession() {
    str fileName = file_dialog::openFileName(
        this, "Load session", QDir::current().absolutePath(), "Session files (*.ste)");
    if (fileName.isEmpty()) {
        TR("load session aborted");
        return;
    }
    try {
        TR("going to load session from file '"+fileName+"'");
        sessionFromFile(fileName);
    } catch(Exception& ex) {
        qWarning() << "Could not load session from file " << fileName << ":\n"
                   << ex.msg() << "\n"
                   << "The application may now be in an inconsistent state.\n"
                   << "Please consider to quit the application, and start afresh.\n";
        gSession->clear();
    }
}

void MainWin::saveSession() {
    str fileName = file_dialog::saveFileName(
        this, "Save session", QDir::current().absolutePath(), "Session files (*.ste)");
    if (!fileName.endsWith(".ste"))
        fileName += ".ste";
    saveSessionTo(QFileInfo(fileName));
}

void MainWin::execCommand(str line) {
    QStringList argv = line.split(" ");
    str cmd = argv.at(0);
    if (cmd=="loadSession") {
        sessionFromFile(argv.at(1));
    } else if (cmd=="quit") {
        close();
    } else {
        qDebug() << "Unknown command: " << line;
    }
}

void MainWin::closeEvent(QCloseEvent* event) {
    qDebug() << "close event";
    saveSettings();
    event->accept();
}

void MainWin::readSettings() {
    if (initialState_.isEmpty())
        initialState_ = saveState();

    Settings s("MainWin");
    restoreGeometry(s.value("geometry").toByteArray());
    restoreState(s.value("state").toByteArray());
}

void MainWin::saveSettings() {
    Settings s("MainWin");
    s.setValue("geometry", saveGeometry());
    s.setValue("state", saveState());
}

void MainWin::viewStatusbar(bool on) {
    statusBar()->setVisible(on);
    toggle_viewStatusbar->setChecked(on);
}

void MainWin::viewFullScreen(bool on) {
    if (on)
        showFullScreen();
    else
        showNormal();
#ifndef Q_OS_OSX
    toggle_fullScreen->setChecked(on);
#endif
}

void MainWin::viewFiles(bool on) {
    dockFiles_->setVisible(on);
    toggle_viewFiles->setChecked(on);
}

void MainWin::viewDatasets(bool on) {
    dockMeasurements_->setVisible(on);
    toggle_viewDatasets->setChecked(on);
}

void MainWin::viewMetadata(bool on) {
    dockDatasetInfo_->setVisible(on);
    toggle_viewMetadata->setChecked(on);
}

void MainWin::viewReset() {
    restoreState(initialState_);
    viewStatusbar(true);
    viewFullScreen(false);
    viewFiles(true);
    viewDatasets(true);
    viewMetadata(true);
}

void MainWin::saveSessionTo(QFileInfo const& fileInfo) const {
    QFile* file = newQ::OutputFile("file", gMainWin, fileInfo.filePath());
    if (!file)
        return;
    QDir::setCurrent(fileInfo.absolutePath());
    const int result = file->write(serializeSession());
    if (!(result >= 0)) THROW("Could not write session");
}

QByteArray MainWin::serializeSession() const {

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
        { "left", cut.left },
        { "top", cut.top },
        { "right", cut.right },
        { "bottom", cut.bottom } };
    top.insert("cut", sub);

    const ImageTransform& trn = gSession->imageTransform();
    top.insert("image transform", trn.val);

    top.insert("files", gSession->dataset().to_json());
    top.insert("combine", gSession->dataset().binning());

    if (gSession->hasCorrFile()) {
        str absPath = gSession->corrset().raw().fileInfo().absoluteFilePath();
        str relPath = QDir::current().relativeFilePath(absPath);
        top.insert("correction file", relPath);
    }

    // TODO save cluster selection

    top.insert("baseline", gSession->baseline().toJson());
    top.insert("peaks", gSession->peaks().toJson());

    top.insert("averaged intensity ", gSession->intenScaledAvg());
    top.insert("intensity scale", qreal_to_json((qreal)gSession->intenScale()));

    return QJsonDocument(top).toJson();
}

void MainWin::sessionFromFile(rcstr filePath) THROWS {
    QFile file(filePath);
    if (!(file.open(QIODevice::ReadOnly | QIODevice::Text)))
        THROW("Cannot open file for reading: " % filePath);
    QDir::setCurrent(QFileInfo(filePath).absolutePath());
    sessionFromJson(file.readAll());
}

void MainWin::sessionFromJson(QByteArray const& json) THROWS {
    QJsonParseError parseError;
    QJsonDocument doc(QJsonDocument::fromJson(json, &parseError));
    if (!(QJsonParseError::NoError == parseError.error))
        THROW("Error parsing session file");

    TakesLongTime __;

    gSession->clear();
    TR("sessionFromJson: cleared old session");

    JsonObj top(doc.object());

    const QJsonArray& files = top.loadArr("files");
    QStringList paths;
    for (const QJsonValue& file : files) {
        str filePath = file.toString();
        QDir dir(filePath);
        if(!dir.makeAbsolute())
            THROW("Invalid file path: " + filePath);
        paths.append(dir.absolutePath());
    }
    gSession->dataset().addGivenFiles(paths);

    const QJsonArray& sels = top.loadArr("selected files", true);
    vec<int> selIndexes;
    for (const QJsonValue& sel : sels) {
        int i = sel.toInt();
        int index = qBound(0, i, files.count());
        if(i != index)
            THROW(str("Invalid selection index: %1").arg(i));
        selIndexes.append(index);
    }

    std::sort(selIndexes.begin(), selIndexes.end());
    int lastIndex = -1;
    for (int index : selIndexes) {
        if (index >= lastIndex)
            THROW("Duplicate selection index");
        lastIndex = index;
    }

    TR("sessionFromJson: going to collect cluster");
    gSession->dataset().setBinning(top.loadPint("combine", 1));

    TR("sessionFromJson: going to set correction file");
    gSession->corrset().loadFile(top.loadString("correction file", ""));

    TR("sessionFromJson: going to load detector geometry");
    const JsonObj& det = top.loadObj("detector");
    gSession->setGeometry(
        det.loadPreal("distance"), det.loadPreal("pixel size"), det.loadIJ("beam offset"));

    TR("sessionFromJson: going to load image cut");
    const JsonObj& cut = top.loadObj("cut");
    int x1 = cut.loadUint("left"), y1 = cut.loadUint("top"),
         x2 = cut.loadUint("right"), y2 = cut.loadUint("bottom");
    gSession->setImageCut(true, false, ImageCut(x1, y1, x2, y2));
    setImageRotate(ImageTransform(top.loadUint("image transform")));

    TR("sessionFromJson: going to load fit setup");
    gSession->baseline().fromJson(top.loadObj("baseline"));

    bool arg1 = top.loadBool("averaged intensity ", true);
    qreal arg2 = top.loadPreal("intensity scale", 1);
    gSession->setIntenScaleAvg(arg1, arg2);

    TR("sessionFromJson: going to load peaks info");
    const QJsonArray& peaksInfo = top.loadArr("peaks");
    for_i (peaksInfo.count())
        gSession->peaks().add(peaksInfo.at(i).toObject());

    TR("installed session from file");
}

void MainWin::loadCorrFile() {
    if (gSession->corrset().hasFile()) {
        gSession->corrset().removeFile();
    } else {
        QString fileName = file_dialog::openFileName(
            gMainWin, "Set correction file", QDir::current().absolutePath(),
            "Data files (*.dat *.mar*);;All files (*.*)");
        if (fileName.isEmpty())
            return;
        QDir::setCurrent(QFileInfo(fileName).absolutePath());
        gSession->corrset().loadFile(fileName);
    }
}

void MainWin::setFittingTab(eFittingTab tab) { // TODO rm
    emit sigFittingTab((fittingTab_ = tab));
}

void MainWin::setImageRotate(ImageTransform rot) {
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
    gSession->setImageCut(true, false, gSession->imageCut());
}

void MainWin::setImageMirror(bool on) {
    toggle_mirrorImage->setChecked(on);
    gSession->setImageTransformMirror(on);
    emit gSession->sigDetector();
}
