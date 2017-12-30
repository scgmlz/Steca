// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/mainwin.cpp
//! @brief     Implements class MainWin
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "mainwin.h"
#include "../manifest.h"
#include "about.h"
#include "config.h"
#include "filedialog.h"
#include "output/output_diagrams.h"
#include "output/output_diffractograms.h"
#include "output/output_polefigures.h"
#include "panels/dock_dataset.h"
#include "panels/dock_files.h"
#include "panels/dock_metadata.h"
#include "panels/tabs_diffractogram.h"
#include "panels/tabs_images.h"
#include "panels/tabs_setup.h"
#include "settings.h"

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

namespace gui {

MainWin::MainWin() : hub_() {
    setWindowIcon(QIcon(":/icon/retroStier"));
    QDir::setCurrent(QDir::homePath());

    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);

    initMenus();
    initLayout();
    initStatusBar();
    connectActions();

    readSettings();
}

void MainWin::initMenus() {
    auto separator = [this]() {
        auto act = new QAction(this);
        act->setSeparator(true);
        return act;
    };

    auto* mbar = menuBar();
#ifdef Q_OS_OSX
    mbar->setNativeMenuBar(false); // REVIEW
#else
    mbar->setNativeMenuBar(true);
#endif

    menuFile_ = mbar->addMenu("&File");
    menuImage_ = mbar->addMenu("&Image");
    menuDgram_ = mbar->addMenu("Di&ffractogram");
    menuOutput_ = mbar->addMenu("&Output");
    menuView_ = mbar->addMenu("&View");
    menuHelp_ = mbar->addMenu("&Help");

    addActions(
        menuFile_,
        {
            hub_.trigger_addFiles, hub_.trigger_removeFile, separator(), hub_.toggle_enableCorr, hub_.trigger_remCorr,
            separator(), hub_.trigger_loadSession,
            hub_.trigger_saveSession, // TODO add: hub_.trigger_clearSession,
        });

    addActions(
        menuFile_,
        {
#ifndef Q_OS_OSX // Mac puts Quit into the Apple menu
            separator(),
#endif
            hub_.trigger_quit,
        });

    addActions(
        menuView_,
        {
            hub_.toggle_viewFiles, hub_.toggle_viewDatasets, hub_.toggle_viewDatasetInfo, separator(),
#ifndef Q_OS_OSX
            hub_.toggle_fullScreen,
#endif
            hub_.toggle_viewStatusbar, separator(), hub_.trigger_viewReset,
        });

    addActions(
        menuImage_,
        {
            hub_.trigger_rotateImage, hub_.toggle_mirrorImage, hub_.toggle_fixedIntenImage, hub_.toggle_linkCuts,
            hub_.toggle_showOverlay, hub_.toggle_stepScale, hub_.toggle_showBins,
        });

    addActions(
        menuDgram_,
        {
            hub_.toggle_selRegions, hub_.toggle_showBackground, hub_.trigger_clearBackground, hub_.trigger_clearReflections,
            separator(), hub_.trigger_addReflection, hub_.trigger_remReflection, separator(), hub_.toggle_combinedDgram,
            hub_.toggle_fixedIntenDgram,
        });

    addActions(
        menuOutput_,
        {
            hub_.trigger_outputPolefigures, hub_.trigger_outputDiagrams, hub_.trigger_outputDiffractograms,
        });

    addActions(
        menuHelp_,
        {
            hub_.trigger_about,
#ifndef Q_OS_OSX
            separator(), // Mac puts About into the Apple menu
#endif
            hub_.trigger_online, hub_.trigger_checkUpdate,
        });
}

void MainWin::addActions(QMenu* menu, QList<QAction*> actions) {
    debug::ensure(menu);
    menu->addActions(actions);
    str prefix = str("%1: ").arg(menu->title().remove('&'));
    for (auto action : actions)
        action->setToolTip(prefix + action->toolTip());
}

void MainWin::initLayout() {
    addDockWidget(Qt::LeftDockWidgetArea, (dockFiles_ = new panel::DockFiles(hub_)));
    addDockWidget(Qt::LeftDockWidgetArea, (dockDatasets_ = new panel::DockDatasets(hub_)));
    addDockWidget(Qt::LeftDockWidgetArea, (dockDatasetInfo_ = new panel::DockMetadata(hub_)));

    auto splMain = new QSplitter(Qt::Vertical);
    splMain->setChildrenCollapsible(false);

    auto splTop = new QSplitter(Qt::Horizontal);
    splTop->setChildrenCollapsible(false);

    setCentralWidget(splMain);

    splMain->addWidget(splTop);
    splMain->addWidget(new panel::TabsDiffractogram(hub_));
    splMain->setStretchFactor(1, 1);

    splTop->addWidget(new panel::TabsSetup(hub_));
    splTop->addWidget(new panel::TabsImages(hub_));
    splTop->setStretchFactor(1, 1);
}

void MainWin::initStatusBar() {
    statusBar();
}

void MainWin::connectActions() {
    auto connectTrigger = [this](QAction* action, void (MainWin::*fun)()) {
        QObject::connect(action, &QAction::triggered, this, fun);
    };

    auto connectToggle = [this](QAction* action, void (MainWin::*fun)(bool)) {
        QObject::connect(action, &QAction::toggled, this, fun);
    };

    connectTrigger(hub_.trigger_loadSession, &MainWin::loadSession);
    connectTrigger(hub_.trigger_saveSession, &MainWin::saveSession);
    connectTrigger(hub_.trigger_clearSession, &MainWin::clearSession);

    connectTrigger(hub_.trigger_addFiles, &MainWin::addFiles);
    connectTrigger(hub_.toggle_enableCorr, &MainWin::enableCorr);

    connectTrigger(hub_.trigger_quit, &MainWin::close);

    connectTrigger(hub_.trigger_outputPolefigures, &MainWin::outputPoleFigures);
    connectTrigger(hub_.trigger_outputDiagrams, &MainWin::outputDiagrams);
    connectTrigger(hub_.trigger_outputDiffractograms, &MainWin::outputDiffractograms);

    connectTrigger(hub_.trigger_about, &MainWin::about);
    connectTrigger(hub_.trigger_online, &MainWin::online);
    QObject::connect(hub_.trigger_checkUpdate, &QAction::triggered, [this]() {checkUpdate();});

    connectToggle(hub_.toggle_viewStatusbar, &MainWin::viewStatusbar);
#ifndef Q_OS_OSX
    connectToggle(hub_.toggle_fullScreen, &MainWin::viewFullScreen);
#endif

    connectToggle(hub_.toggle_viewFiles, &MainWin::viewFiles);
    connectToggle(hub_.toggle_viewDatasets, &MainWin::viewDatasets);
    connectToggle(hub_.toggle_viewDatasetInfo, &MainWin::viewDatasetInfo);

    connectTrigger(hub_.trigger_viewReset, &MainWin::viewReset);
}

void MainWin::about() {
    AboutBox(this).exec();
}

void MainWin::online() {
    QDesktopServices::openUrl(QUrl(STECA2_PAGES_URL));
}

void MainWin::checkUpdate(bool completeReport) {

    QNetworkRequest req;

    str ver = qApp->applicationVersion();
    str qry = ver % "\t| " % QSysInfo::prettyProductName();
    req.setUrl(QUrl(str(STECA2_VERSION_URL) % "?" % qry));
    auto reply = netMan_.get(req);

    connect(reply, &QNetworkReply::finished, [this, completeReport, reply]() {
        if (QNetworkReply::NoError != reply->error()) {
            messageDialog("Network Error", reply->errorString());
        } else {
            str ver = qApp->applicationVersion();
            str lastVer = reply->readAll().trimmed();

            str name = qApp->applicationName();

            if (ver != lastVer)
                messageDialog(
                    str("%1 update").arg(name),
                    str("<p>The latest released %1 version is %2. You have "
                        "version %3.</p>"
                        "<p><a href='%4'>Get new %1</a></p>")
                        .arg(name, lastVer, ver, STECA2_DOWNLOAD_URL));
            else if (completeReport)
                messageDialog(
                    str("%1 update").arg(name),
                    str("<p>You have the latest released %1 version (%2).</p>").arg(name).arg(ver));
        }
    });
}

void MainWin::messageDialog(rcstr title, rcstr text) {
    QMessageBox::information(this, title, text);
}

void MainWin::show() {
    QMainWindow::show();
    onShow();
}

void MainWin::close() {
    QMainWindow::close();
}

void MainWin::addFiles() {
    QStringList fileNames = file_dialog::openFileNames(
        this, "Add files", QDir::current().absolutePath(),
        "Data files (*.dat *.mar*);;All files (*.*)");
    update();
    if (!fileNames.isEmpty()) {
        QDir::setCurrent(QFileInfo(fileNames.at(0)).absolutePath());
        hub_.addGivenFiles(fileNames);
    }
}

void MainWin::enableCorr() {
    str fileName;
    if (!hub_.hasCorrFile()) {
        fileName = file_dialog::openFileName(
            this, "Set correction file", QDir::current().absolutePath(),
            "Data files (*.dat *.mar*);;All files (*.*)");
        update();
    }
    if (!fileName.isEmpty()) {
        QDir::setCurrent(QFileInfo(fileName).absolutePath());
        hub_.setCorrFile(fileName);
    }
}

void MainWin::loadSession() {
    str fileName = file_dialog::openFileName(
        this, "Load session", QDir::current().absolutePath(),
        "Session files (*.ste);;All files (*.*)");
    update();
    if (fileName.isEmpty()) {
        TR("load session aborted");
        return;
    }
    try {
        TR("going to load session from file '"+fileName+"'");
        hub_.sessionFromFile(QFileInfo(fileName));
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
        this, "Save session", QDir::current().absolutePath(),
        "Session files (*.ste);;All files (*.*)");
    update();
    if (!fileName.endsWith(".ste"))
        fileName += ".ste";
    hub_.saveSession(QFileInfo(fileName));
}

void MainWin::clearSession() {
    hub_.clearSession();
}

void MainWin::outputPoleFigures() {
    auto popup = new output::PoleFiguresFrame(hub_, "Pole Figures", this);
    popup->show();
}

void MainWin::outputDiagrams() {
    auto popup = new output::DiagramsFrame(hub_, "Diagrams", this);
    popup->show();
}

void MainWin::outputDiffractograms() {
    auto popup = new output::DiffractogramsFrame(hub_, "Diffractograms", this);
    popup->show();
}

void MainWin::closeEvent(QCloseEvent* event) {
    onClose();
    event->accept();
}

void MainWin::onShow() {
    checkActions();
    hub_.clearSession();

#ifdef DEVELOPMENT
    // automatic actions - load files & open dialog
    // helps with development

    auto safeLoad = [this](rcstr fileName) {
        QFileInfo info(fileName);
        if (info.exists())
            hub_.loadSession(info);
    };
    safeLoad("/home/jan/C/+dev/fz/data/0.ste");
    hub_.actions.outputPolefigures->trigger();
#endif

    Settings s(config_key::GROUP_CONFIG);
    auto ver = qApp->applicationVersion();
    if (s.readStr(config_key::CURRENT_VERSION) != ver) {
        // new version
        s.saveStr(config_key::CURRENT_VERSION, ver);
        s.saveBool(config_key::STARTUP_CHECK_UPDATE, true);
        s.saveBool(config_key::STARTUP_ABOUT, true);
    }

    if (s.readBool(config_key::STARTUP_CHECK_UPDATE, true))
        checkUpdate(false);
    if (s.readBool(config_key::STARTUP_ABOUT, true))
        about();
}

void MainWin::onClose() {
    saveSettings();
}

void MainWin::readSettings() {
    if (initialState_.isEmpty())
        initialState_ = saveState();

    Settings s(config_key::GROUP_MAINWIN);
    restoreGeometry(s.value(config_key::GEOMETRY).toByteArray());
    restoreState(s.value(config_key::STATE).toByteArray());
}

void MainWin::saveSettings() {
    Settings s(config_key::GROUP_MAINWIN);
    s.setValue(config_key::GEOMETRY, saveGeometry());
    s.setValue(config_key::STATE, saveState());
}

void MainWin::checkActions() {
    hub_.toggle_viewStatusbar->setChecked(statusBar()->isVisible());

#ifndef Q_OS_OSX
    hub_.toggle_fullScreen->setChecked(isFullScreen());
#endif

    hub_.toggle_viewFiles->setChecked(dockFiles_->isVisible());
    hub_.toggle_viewDatasets->setChecked(dockDatasets_->isVisible());
    hub_.toggle_viewDatasetInfo->setChecked(dockDatasetInfo_->isVisible());
}

void MainWin::viewStatusbar(bool on) {
    statusBar()->setVisible(on);
    hub_.toggle_viewStatusbar->setChecked(on);
}

void MainWin::viewFullScreen(bool on) {
    if (on)
        showFullScreen();
    else
        showNormal();

#ifndef Q_OS_OSX
    hub_.toggle_fullScreen->setChecked(on);
#endif
}

void MainWin::viewFiles(bool on) {
    dockFiles_->setVisible(on);
    hub_.toggle_viewFiles->setChecked(on);
}

void MainWin::viewDatasets(bool on) {
    dockDatasets_->setVisible(on);
    hub_.toggle_viewDatasets->setChecked(on);
}

void MainWin::viewDatasetInfo(bool on) {
    dockDatasetInfo_->setVisible(on);
    hub_.toggle_viewDatasetInfo->setChecked(on);
}

void MainWin::viewReset() {
    restoreState(initialState_);
    viewStatusbar(true);
    viewFullScreen(false);
    viewFiles(true);
    viewDatasets(true);
    viewDatasetInfo(true);
}

} // namespace gui
