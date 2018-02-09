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
#include "gui/base/filedialog.h"
#include <QApplication>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QMenuBar>
#include <QMessageBox>
#include <QNetworkReply>
#include <QSplitter>
#include <QStatusBar>
#include <QStringBuilder> // for ".." % ..

MainWin* gGui; //!< global pointer to _the_ main window

// ************************************************************************** //
//  class MainWin
// ************************************************************************** //

MainWin::MainWin()
    : isFixedIntenImageScale_(false)
    , isFixedIntenDgramScale_(false)
    , isCombinedDgram_(false)
    , settings_("main_settings")
{
    gSession = Session::instance();
    gConsole = Console::instance();
    gGui = this;

    setWindowIcon(QIcon(":/icon/retroStier"));
    QDir::setCurrent(QDir::homePath());
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);

    // create actions

    trigger_about = newT::Trigger("trigger_about", "About " + qApp->applicationName());

    trigger_online = newT::Trigger("trigger_online", "Open docs in external browser");

    trigger_checkUpdate = newT::Trigger("trigger_checkUpdate", "Check for update");

    trigger_quit = newT::Trigger("trigger_quit", "Quit");
    trigger_quit->setShortcut(QKeySequence::Quit);

    toggle_viewStatusbar = newT::Toggle("toggle_viewStatusbar", "Statusbar", true);
    toggle_viewStatusbar->setShortcut(Qt::Key_F12);

    toggle_viewFiles = newT::Toggle("toggle_viewFiles", "Files", true);
    toggle_viewFiles->setShortcut(Qt::Key_F8);

    toggle_viewDatasets = newT::Toggle("toggle_viewDatasets", "Datasets", true);
    toggle_viewDatasets->setShortcut(Qt::Key_F9);

    toggle_viewMetadata = newT::Toggle("toggle_viewMetadata", "Metadata", true);
    toggle_viewMetadata->setShortcut(Qt::Key_F10);

    trigger_viewReset = newT::Trigger("trigger_viewReset", "Reset");

#ifndef Q_OS_OSX
    toggle_fullScreen = newT::Toggle("toggle_fullScreen", "FullScreen", false);
    toggle_fullScreen->setShortcut(Qt::Key_F11);
#endif

    trigger_loadSession = newT::Trigger("trigger_loadSession", "Load session...");

    trigger_saveSession = newT::Trigger("trigger_saveSession", "Save session...");

    trigger_clearSession = newT::Trigger("trigger_clearSession", "Clear session");

    trigger_addFiles = newT::Trigger("trigger_addFiles", "Add files...", ":/icon/add");
    trigger_addFiles->setShortcut(Qt::CTRL | Qt::Key_O);

    trigger_removeFile = newT::Trigger(
        "trigger_removeFile", "Remove highlighted file", ":/icon/rem");
    trigger_removeFile->setShortcut(QKeySequence::Delete);
    trigger_removeFile->setEnabled(false);
    QObject::connect(trigger_removeFile, &QAction::triggered, []() {
            gSession->dataset().removeFile(); });

    trigger_corrFile = newT::Trigger("trigger_corrFile", "Add correction file", ":/icon/add");
    trigger_corrFile->setShortcut(Qt::SHIFT | Qt::CTRL | Qt::Key_O);
    connect(trigger_corrFile, &QAction::triggered, this, &MainWin::loadCorrFile);
    toggle_enableCorr = newT::Toggle(
        "toggle_enableCorr", "Enable correction file", false, ":/icon/useCorrection");
    connect(toggle_enableCorr, &QAction::toggled, [this](bool on) {
            gSession->corrset().tryEnable(on); });
    trigger_rotateImage = newT::Trigger("trigger_rotateImage", "Rotate", ":/icon/rotate0");
    trigger_rotateImage->setShortcut(Qt::CTRL | Qt::Key_R);
    connect(trigger_rotateImage, &QAction::triggered, [this]() {
        setImageRotate(gSession->imageTransform().nextRotate()); });

    toggle_mirrorImage = newT::Toggle("toggle_mirrorImage", "Mirror", false, ":/icon/mirrorHorz");
    connect(toggle_mirrorImage, &QAction::toggled, [this](bool on) { setImageMirror(on); });

    toggle_linkCuts = newT::Toggle("toggle_linkCuts", "Link cuts", false, ":/icon/link");

    toggle_showOverlay = newT::Toggle("toggle_showOverlay", "Show overlay", false, ":/icon/crop");

    toggle_stepScale = newT::Toggle("toggle_stepScale", "Scale in steps", false, ":/icon/steps");

    toggle_showBins = newT::Toggle("toggle_showBins", "Show bins", false, ":/icon/angle");

    toggle_fixedIntenImage = newT::Toggle(
        "toggle_fixedIntenImage", "Global intensity scale", false, ":/icon/scale");
    connect(toggle_fixedIntenImage, &QAction::toggled, [this](bool on) {
        isFixedIntenImageScale_ = on;
        emit gSession->sigImage();
        emit gSession->sigDiffractogram();
        });

    toggle_fixedIntenDgram = newT::Toggle(
        "toggle_fixedIntenDgram", "Fixed intensity scale", false);
    connect(toggle_fixedIntenDgram, &QAction::toggled, [this](bool on) {
        isFixedIntenDgramScale_ = on;
        emit gSession->sigImage();
        emit gSession->sigDiffractogram();
        });

    toggle_combinedDgram = newT::Toggle("toggle_combinedDgram", "All measurements", true);
    toggle_combinedDgram->setChecked(false);
    connect(toggle_combinedDgram, &QAction::toggled, [this](bool on) {
        isCombinedDgram_ = on;
        emit gSession->sigImage();
        emit gSession->sigDiffractogram();
        });

    toggle_selRegions = newT::Toggle(
        "toggle_selRegions", "Select regions", false, ":/icon/selRegion");
    // for peaks, ->setIcon(QIcon(":/icon/reflRegion"));

    toggle_showBackground = newT::Toggle(
        "toggle_showBackground", "Show fitted background", false, ":/icon/showBackground");

    trigger_clearBackground = newT::Trigger(
        "trigger_clearBackground", "Clear background regions", ":/icon/clear");
    connect(trigger_clearBackground, &QAction::triggered, [this]() {
            gSession->baseline().setRanges({}); });

    trigger_clearPeaks = newT::Trigger("trigger_clearPeaks", "Clear peaks", ":/icon/clear");

    trigger_addPeak = newT::Trigger("trigger_addPeak", "Add peak", ":/icon/add");

    trigger_removePeak = newT::Trigger("trigger_removePeak", "Remove peak", ":/icon/rem");
    trigger_removePeak->setEnabled(false);

    trigger_outputPolefigures = newT::Trigger("trigger_outputPolefigures", "Pole figures...");

    trigger_outputDiagrams = newT::Trigger("trigger_outputDiagrams", "Diagrams...");

    trigger_outputDiffractograms = newT::Trigger(
        "trigger_outputDiffractograms", "Diffractograms...");

    // connect signals
    QObject::connect(gSession, &Session::sigFiles, [this]() {
            trigger_removeFile->setEnabled(gSession->dataset().countFiles());
        });
    QObject::connect(gSession, &Session::sigCorr, [this]() {
            bool hasFile = gSession->hasCorrFile();
            trigger_corrFile->setIcon(QIcon(hasFile ? ":/icon/rem" : ":/icon/add"));
            QString text = QString(hasFile ? "Remove" : "Add") + " correction file";
            trigger_corrFile->setText(text);
            trigger_corrFile->setToolTip(text.toLower());
            toggle_enableCorr->setEnabled(gSession->hasCorrFile());
            toggle_enableCorr->setChecked(gSession->corrset().isEnabled());
            emit gSession->sigDiffractogram();
            emit gSession->sigImage();
        });
    QObject::connect(gSession, &Session::sigPeaks, [this]() {
            trigger_removePeak->setEnabled(gSession->peaks().count());
            trigger_outputPolefigures->setEnabled(gSession->peaks().count());
            emit gSession->sigDiffractogram();
        });

    initMenu();
    initLayout();
    connectActions();
    readSettings();

    saveDir = settings_.readStr("export_directory");
    saveFmt = settings_.readStr("export_format");
}

MainWin::~MainWin() {
    settings_.saveStr("export_directory", saveDir);
    settings_.saveStr("export_format", saveFmt);
}

//! Initialize the menu bar.
void MainWin::initMenu() {

    QMenuBar* mbar = menuBar();

    auto _separator = [mbar]()->QAction* {
        QAction* ret = new QAction(mbar);
        ret->setSeparator(true);
        return ret;
    };

    auto _actionsToMenu = [mbar](const char* menuName, QList<QAction*> actions)->QMenu* {
        QMenu* menu = mbar->addMenu(menuName);
        menu->addActions(actions);
        QString prefix = QString("%1: ").arg(menu->title().remove('&'));
        for (auto action : actions)
            action->setToolTip(prefix + action->toolTip());
        return menu;
    };

#ifdef Q_OS_OSX
    mbar->setNativeMenuBar(false);
#else
    mbar->setNativeMenuBar(true);
#endif

    _actionsToMenu(
        "&File",
        {   trigger_addFiles,
                trigger_removeFile,
                _separator(),
                trigger_corrFile,
                toggle_enableCorr,
                _separator(),
                trigger_loadSession,
                trigger_saveSession,
                trigger_clearSession,
#ifndef Q_OS_OSX // Mac puts Quit into the Apple menu
                _separator(),
#endif
                trigger_quit,
        });

    QMenu* menuImage = _actionsToMenu(
        "&Image",
        {   trigger_rotateImage,
                toggle_mirrorImage,
                toggle_fixedIntenImage,
                toggle_linkCuts,
                toggle_showOverlay,
                toggle_stepScale,
                toggle_showBins,
        });
    menuImage->setEnabled(false);
    QObject::connect(gSession, &Session::sigFiles, [menuImage]()
                     { menuImage->setEnabled(gSession->dataset().countFiles()); });

    QMenu* menuDgram = _actionsToMenu(
        "&Diffractogram",
        {   toggle_selRegions,
                toggle_showBackground,
                trigger_clearBackground,
                trigger_clearPeaks,
                _separator(),
                trigger_addPeak,
                trigger_removePeak,
                _separator(),
                toggle_combinedDgram,
                toggle_fixedIntenDgram,
        });
    menuDgram->setEnabled(false);
    QObject::connect(gSession, &Session::sigFiles, [menuDgram]()
                     { menuDgram->setEnabled(gSession->dataset().countFiles()); });

    QMenu* menuOutput = _actionsToMenu(
        "&Output",
        {   trigger_outputPolefigures,
                trigger_outputDiagrams,
                trigger_outputDiffractograms,
        });
    menuOutput->setEnabled(false);
    QObject::connect(gSession, &Session::sigActivated, [menuOutput]()
                     { menuOutput->setEnabled(gSession->experiment().size()); });

    _actionsToMenu(
        "&View",
        {   toggle_viewFiles,
                toggle_viewDatasets,
                toggle_viewMetadata,
                _separator(),
#ifndef Q_OS_OSX
                toggle_fullScreen,
#endif
                toggle_viewStatusbar,
                _separator(),
                trigger_viewReset,
        });

    _actionsToMenu(
        "&Help",
        {   trigger_about, // Mac puts About into the Apple menu
                trigger_online,
                trigger_checkUpdate,
        });
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

    QString ver = qApp->applicationVersion();
    QString qry = ver % "\t| " % QSysInfo::prettyProductName();
    req.setUrl(QUrl(QString(STECA2_VERSION_URL) % "?" % qry));
    QNetworkReply* reply = netMan_.get(req);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (QNetworkReply::NoError != reply->error()) {
            qWarning() << "Network Error: " << reply->errorString();
            return;
        }
        QString ver = qApp->applicationVersion();
        QString lastVer = reply->readAll().trimmed();
        QString name = qApp->applicationName();
        QString result;
        if (ver != lastVer)
            result = QString(
                "<p>The latest released %1 version is %2. You have "
                "version %3.</p>"
                "<p><a href='%4'>Open download location in external browser</a></p>")
                .arg(name, lastVer, ver, STECA2_DOWNLOAD_URL);
        else
            result = QString(
                "<p>You have the latest released %1 version (%2).</p>").arg(name).arg(ver);
        QMessageBox::information(this, QString("%1 update").arg(name), result);
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
    TakesLongTime __;
    gSession->dataset().addGivenFiles(fileNames);
}

void MainWin::loadSession() {
    QString fileName = file_dialog::openFileName(
        this, "Load session", QDir::current().absolutePath(), "Session files (*.ste)");
    if (fileName.isEmpty())
        return;
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
    QString fileName = file_dialog::saveFileName(
        this, "Save session", QDir::current().absolutePath(), "Session files (*.ste)");
    if (!fileName.endsWith(".ste"))
        fileName += ".ste";
    saveSessionTo(QFileInfo(fileName));
}

void MainWin::closeEvent(QCloseEvent* event) {
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

void MainWin::saveSessionTo(QFileInfo const& fileInfo) {
    QFile* file = file_dialog::OutputFile("file", this, fileInfo.filePath());
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
        QString absPath = gSession->corrset().raw().fileInfo().absoluteFilePath();
        QString relPath = QDir::current().relativeFilePath(absPath);
        top.insert("correction file", relPath);
    }

    // TODO save cluster selection

    top.insert("baseline", gSession->baseline().toJson());
    top.insert("peaks", gSession->peaks().toJson());

    top.insert("averaged intensity ", gSession->intenScaledAvg());
    top.insert("intensity scale", qreal_to_json((qreal)gSession->intenScale()));

    return QJsonDocument(top).toJson();
}

void MainWin::sessionFromFile(const QString& filePath) THROWS {
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
        QString filePath = file.toString();
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
            THROW(QString("Invalid selection index: %1").arg(i));
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
            this, "Set correction file", QDir::current().absolutePath(),
            "Data files (*.dat *.mar*);;All files (*.*)");
        if (fileName.isEmpty())
            return;
        QDir::setCurrent(QFileInfo(fileName).absolutePath());
        gSession->corrset().loadFile(fileName);
    }
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
