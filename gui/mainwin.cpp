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

#include "gui/mainwin.h"
#include "../manifest.h"
#include "gui/popup/about.h"
#include "gui/popup/filedialog.h"
#include "gui/output/output_diagrams.h"
#include "gui/output/output_diffractograms.h"
#include "gui/output/output_polefigures.h"
#include "gui/panels/dock_dataset.h"
#include "gui/panels/dock_files.h"
#include "gui/panels/dock_metadata.h"
#include "gui/panels/tabs_diffractogram.h"
#include "gui/panels/tabs_images.h"
#include "gui/panels/tabs_setup.h"
#include "core/session.h"
#include "gui/cfg/settings.h"
#include "gui/thehub.h"

#include <QApplication>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDir>
#include <QMenuBar>
#include <QMessageBox>
#include <QNetworkReply>
#include <QSplitter>
#include <QStatusBar>
#include <QStringBuilder> // for ".." % ..

TheHub* gHub; //!< global, for signalling and command flow

// ************************************************************************** //
//  file-scoped functions
// ************************************************************************** //

namespace {

//! Initialize the menu bar. Part of the MainWin initialization.
void initMenus(QMenuBar* mbar) {

    auto separator = [mbar]()->QAction* {
        QAction* ret = new QAction(mbar);
        ret->setSeparator(true);
        return ret;
    };

    auto actionsToMenu = [mbar](const char* menuName, QList<QAction*> actions)->void {
        QMenu* menu = mbar->addMenu(menuName);
        menu->addActions(actions);
        str prefix = str("%1: ").arg(menu->title().remove('&'));
        for (auto action : actions)
            action->setToolTip(prefix + action->toolTip());
    };

#ifdef Q_OS_OSX
    mbar->setNativeMenuBar(false); // REVIEW
#else
    mbar->setNativeMenuBar(true);
#endif

    actionsToMenu(
        "&File",
        {
            gHub->trigger_addFiles,
                gHub->trigger_removeFile,
                separator(),
                gHub->toggle_enableCorr,
                gHub->trigger_remCorr,
                separator(),
                gHub->trigger_loadSession,
                gHub->trigger_saveSession,
                gHub->trigger_clearSession,
#ifndef Q_OS_OSX // Mac puts Quit into the Apple menu
                separator(),
#endif
                gHub->trigger_quit,
        });

    actionsToMenu(
        "&Image",
        {   gHub->trigger_rotateImage,
                gHub->toggle_mirrorImage,
                gHub->toggle_fixedIntenImage,
                gHub->toggle_linkCuts,
                gHub->toggle_showOverlay,
                gHub->toggle_stepScale,
                gHub->toggle_showBins,
        });

    actionsToMenu(
        "&Diffractogram",
        {
            gHub->toggle_selRegions,
                gHub->toggle_showBackground,
                gHub->trigger_clearBackground,
                gHub->trigger_clearReflections,
                separator(),
                gHub->trigger_addReflection,
                gHub->trigger_remReflection,
                separator(),
                gHub->toggle_combinedDgram,
                gHub->toggle_fixedIntenDgram,
        });

    actionsToMenu(
        "&Output",
        {
            gHub->trigger_outputPolefigures,
                gHub->trigger_outputDiagrams,
                gHub->trigger_outputDiffractograms,
        });

    actionsToMenu(
        "&View",
        {   gHub->toggle_viewFiles,
                gHub->toggle_viewDatasets,
                gHub->toggle_viewMetadata,
                separator(),
#ifndef Q_OS_OSX
                gHub->toggle_fullScreen,
#endif
                gHub->toggle_viewStatusbar,
                separator(),
                gHub->trigger_viewReset,
        });

    actionsToMenu(
        "&Help",
        {
            gHub->trigger_about, // Mac puts About into the Apple menu
                gHub->trigger_online,
                gHub->trigger_checkUpdate,
        });
}

} // anonymous namespace

// ************************************************************************** //
//  class MainWin
// ************************************************************************** //

MainWin::MainWin() {
    qDebug() << "MainWin/";
    gHub = new TheHub();
    setWindowIcon(QIcon(":/icon/retroStier"));
    QDir::setCurrent(QDir::homePath());
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);

    initMenus(menuBar());
    initLayout();
    connectActions();
    readSettings();

    qDebug() << "/MainWin";
}


void MainWin::initLayout() {
    addDockWidget(Qt::LeftDockWidgetArea, (dockFiles_ = new SubframeFiles()));
    addDockWidget(Qt::LeftDockWidgetArea, (dockDatasets_ = new SubframeDatasets()));
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

    connectTrigger(gHub->trigger_loadSession, &MainWin::loadSession);
    connectTrigger(gHub->trigger_saveSession, &MainWin::saveSession);
    connectTrigger(gHub->trigger_clearSession, &MainWin::clearSession);

    connectTrigger(gHub->trigger_addFiles, &MainWin::addFiles);
    connectTrigger(gHub->toggle_enableCorr, &MainWin::enableCorr);

    connectTrigger(gHub->trigger_quit, &MainWin::close);

    QObject::connect(gHub->trigger_outputPolefigures, &QAction::triggered,
                     [this](){PoleFiguresFrame("Pole Figures", this).exec();});
    QObject::connect(gHub->trigger_outputDiagrams, &QAction::triggered,
                     [this](){DiagramsFrame("Diagrams", this).exec();});
    QObject::connect(gHub->trigger_outputDiffractograms, &QAction::triggered,
                     [this](){DiffractogramsFrame("Diffractograms", this).exec();});

    QObject::connect(gHub->trigger_about, &QAction::triggered, [this](){AboutBox(this).exec();});
    connectTrigger(gHub->trigger_online, &MainWin::online);
    connectTrigger(gHub->trigger_checkUpdate, &MainWin::checkUpdate);

    connectToggle(gHub->toggle_viewStatusbar, &MainWin::viewStatusbar);
#ifndef Q_OS_OSX
    connectToggle(gHub->toggle_fullScreen, &MainWin::viewFullScreen);
#endif

    connectToggle(gHub->toggle_viewFiles, &MainWin::viewFiles);
    connectToggle(gHub->toggle_viewDatasets, &MainWin::viewDatasets);
    connectToggle(gHub->toggle_viewMetadata, &MainWin::viewMetadata);

    connectTrigger(gHub->trigger_viewReset, &MainWin::viewReset);
}

void MainWin::online() {
    QDesktopServices::openUrl(QUrl(STECA2_PAGES_URL));
}

void MainWin::checkUpdate() {

    QNetworkRequest req;

    str ver = qApp->applicationVersion();
    str qry = ver % "\t| " % QSysInfo::prettyProductName();
    req.setUrl(QUrl(str(STECA2_VERSION_URL) % "?" % qry));
    auto reply = netMan_.get(req);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (QNetworkReply::NoError != reply->error()) {
            messageDialog("Network Error", reply->errorString());
        } else {
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
            messageDialog(str("%1 update").arg(name), result);
        }
    });
}

void MainWin::messageDialog(rcstr title, rcstr text) {
    QMessageBox::information(this, title, text);
}

void MainWin::close() {
    qDebug() << "Good bye\n";
    QMainWindow::close();
}

void MainWin::addFiles() {
    QStringList fileNames = file_dialog::openFileNames(
        this, "Add files", QDir::current().absolutePath(),
        "Data files (*.dat *.mar*);;All files (*.*)");
    update();
    if (!fileNames.isEmpty()) {
        QDir::setCurrent(QFileInfo(fileNames.at(0)).absolutePath());
        gHub->addGivenFiles(fileNames);
    }
}

void MainWin::enableCorr() {
    str fileName;
    if (!gSession->hasCorrFile()) {
        fileName = file_dialog::openFileName(
            this, "Set correction file", QDir::current().absolutePath(),
            "Data files (*.dat *.mar*);;All files (*.*)");
        update();
    }
    if (!fileName.isEmpty()) {
        QDir::setCurrent(QFileInfo(fileName).absolutePath());
        gHub->setCorrFile(fileName);
    }
}

void MainWin::loadSession() {
    str fileName = file_dialog::openFileName(
        this, "Load session", QDir::current().absolutePath(), "Session files (*.ste)");
    update();
    if (fileName.isEmpty()) {
        TR("load session aborted");
        return;
    }
    try {
        TR("going to load session from file '"+fileName+"'");
        gHub->sessionFromFile(fileName);
    } catch(Exception& ex) {
        qWarning() << "Could not load session from file " << fileName << ":\n"
                   << ex.msg() << "\n"
                   << "The application may now be in an inconsistent state.\n"
                   << "Please consider to quit the application, and start afresh.\n";
        clearSession();
    }
}

void MainWin::saveSession() {
    str fileName = file_dialog::saveFileName(
        this, "Save session", QDir::current().absolutePath(), "Session files (*.ste)");
    update();
    if (!fileName.endsWith(".ste"))
        fileName += ".ste";
    gHub->saveSession(QFileInfo(fileName));
}

void MainWin::clearSession() {
    gSession->clear();
    emit gHub->sigFilesSelected();
    emit gHub->sigSuitesChanged();
}

void MainWin::execCommand(str line) {
    QStringList argv = line.split(" ");
    str cmd = argv.at(0);
    if (cmd=="loadSession") {
        gHub->sessionFromFile(argv.at(1));
    } else if (cmd=="quit") {
        close();
    } else {
        qDebug() << "Unknown command: " << line << "\n";
    }
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
    gHub->toggle_viewStatusbar->setChecked(on);
}

void MainWin::viewFullScreen(bool on) {
    if (on)
        showFullScreen();
    else
        showNormal();
#ifndef Q_OS_OSX
    gHub->toggle_fullScreen->setChecked(on);
#endif
}

void MainWin::viewFiles(bool on) {
    dockFiles_->setVisible(on);
    gHub->toggle_viewFiles->setChecked(on);
}

void MainWin::viewDatasets(bool on) {
    dockDatasets_->setVisible(on);
    gHub->toggle_viewDatasets->setChecked(on);
}

void MainWin::viewMetadata(bool on) {
    dockDatasetInfo_->setVisible(on);
    gHub->toggle_viewMetadata->setChecked(on);
}

void MainWin::viewReset() {
    restoreState(initialState_);
    viewStatusbar(true);
    viewFullScreen(false);
    viewFiles(true);
    viewDatasets(true);
    viewMetadata(true);
}
