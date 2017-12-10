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
#include "app.h"
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

#include <QDesktopServices>
#include <QDir>
#include <QMenuBar>
#include <QMessageBox>
#include <QNetworkReply>
#include <QSplitter>
#include <QStatusBar>

namespace gui {

MainWin::MainWin() : hub_(), acts_(hub_.actions) {
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
            acts_.addFiles, acts_.remFile, separator(), acts_.enableCorr, acts_.remCorr,
            separator(), acts_.loadSession,
            acts_.saveSession, // TODO add: acts_.clearSession,
        });

    addActions(
        menuFile_,
        {
#ifndef Q_OS_OSX // Mac puts Quit into the Apple menu
            separator(),
#endif
            acts_.quit,
        });

    addActions(
        menuView_,
        {
            acts_.viewFiles, acts_.viewDatasets, acts_.viewDatasetInfo, separator(),
#ifndef Q_OS_OSX
            acts_.fullScreen,
#endif
            acts_.viewStatusbar, separator(), acts_.viewReset,
        });

    addActions(
        menuImage_,
        {
            acts_.rotateImage, acts_.mirrorImage, acts_.fixedIntenImage, acts_.linkCuts,
            acts_.showOverlay, acts_.stepScale, acts_.showBins,
        });

    addActions(
        menuDgram_,
        {
            acts_.selRegions, acts_.showBackground, acts_.clearBackground, acts_.clearReflections,
            separator(), acts_.addReflection, acts_.remReflection, separator(), acts_.combinedDgram,
            acts_.fixedIntenDgram,
        });

    addActions(
        menuOutput_,
        {
            acts_.outputPolefigures, acts_.outputDiagrams, acts_.outputDiffractograms,
        });

    addActions(
        menuHelp_,
        {
            acts_.about,
#ifndef Q_OS_OSX
            separator(), // Mac puts About into the Apple menu
#endif
            acts_.online, acts_.checkUpdate,
        });
}

void MainWin::addActions(QMenu* menu, QList<QAction*> actions) {
    EXPECT(menu)
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
    auto onTrigger = [this](QAction* action, void (MainWin::*fun)()) {
        QObject::connect(action, &QAction::triggered, this, fun);
    };

    auto onToggle = [this](QAction* action, void (MainWin::*fun)(bool)) {
        QObject::connect(action, &QAction::toggled, this, fun);
    };

    onTrigger(acts_.loadSession, &MainWin::loadSession);
    onTrigger(acts_.saveSession, &MainWin::saveSession);
    onTrigger(acts_.clearSession, &MainWin::clearSession);

    onTrigger(acts_.addFiles, &MainWin::addFiles);
    onTrigger(acts_.enableCorr, &MainWin::enableCorr);

    onTrigger(acts_.quit, &MainWin::close);

    onTrigger(acts_.outputPolefigures, &MainWin::outputPoleFigures);
    onTrigger(acts_.outputDiagrams, &MainWin::outputDiagrams);
    onTrigger(acts_.outputDiffractograms, &MainWin::outputDiffractograms);

    onTrigger(acts_.about, &MainWin::about);
    onTrigger(acts_.online, &MainWin::online);
    onTrigger(acts_.checkUpdate, &MainWin::checkUpdate);

    onToggle(acts_.viewStatusbar, &MainWin::viewStatusbar);
#ifndef Q_OS_OSX
    onToggle(acts_.fullScreen, &MainWin::viewFullScreen);
#endif

    onToggle(acts_.viewFiles, &MainWin::viewFiles);
    onToggle(acts_.viewDatasets, &MainWin::viewDatasets);
    onToggle(acts_.viewDatasetInfo, &MainWin::viewDatasetInfo);

    onTrigger(acts_.viewReset, &MainWin::viewReset);
}

void MainWin::about() {
    AboutBox(this).exec();
}

void MainWin::online() {
    QDesktopServices::openUrl(QUrl(STECA2_PAGES_URL));
}

void MainWin::checkUpdate() {
    checkUpdate(true);
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
    super::show();
    onShow();
}

void MainWin::close() {
    super::close();
}

void MainWin::addFiles() {
    DM("MainWin::addFiles beg")
    str_lst fileNames = file_dialog::openFileNames(
        this, "Add files", QDir::current().absolutePath(),
        "Data files (*.dat *.mar*);;All files (*.*)");

    if (!fileNames.isEmpty()) {
        QDir::setCurrent(QFileInfo(fileNames.at(0)).absolutePath());
        DM("MainWin::addFiles call Hub")
        hub_.addGivenFiles(fileNames);
    }
    DM("MainWin::addFiles end")
}

void MainWin::enableCorr() {
    DM("MainWin::enableCorr beg")
    str fileName;
    if (!hub_.hasCorrFile())
        fileName = file_dialog::openFileName(
            this, "Set correction file", QDir::current().absolutePath(),
            "Data files (*.dat *.mar*);;All files (*.*)");

    if (!fileName.isEmpty()) {
        QDir::setCurrent(QFileInfo(fileName).absolutePath());
        DM("MainWin::enableCorr call Hub")
        hub_.setCorrFile(fileName);
    }
    DM("MainWin::enableCorr end")
}

static str const STE(".ste");

void MainWin::loadSession() {
    str fileName = file_dialog::openFileName(
        this, "Load session", QDir::current().absolutePath(),
        "Session files (*" % STE % ");;All files (*.*)");

    if (fileName.isEmpty())
        return;

    hub_.loadSession(QFileInfo(fileName));
}

void MainWin::saveSession() {
    str fileName = file_dialog::saveFileName(
        this, "Save session", QDir::current().absolutePath(),
        "Session files (*" % STE % ");;All files (*.*)");

    if (!fileName.endsWith(STE))
        fileName += STE;
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
    acts_.viewStatusbar->setChecked(statusBar()->isVisible());

#ifndef Q_OS_OSX
    acts_.fullScreen->setChecked(isFullScreen());
#endif

    acts_.viewFiles->setChecked(dockFiles_->isVisible());
    acts_.viewDatasets->setChecked(dockDatasets_->isVisible());
    acts_.viewDatasetInfo->setChecked(dockDatasetInfo_->isVisible());
}

void MainWin::viewStatusbar(bool on) {
    statusBar()->setVisible(on);
    acts_.viewStatusbar->setChecked(on);
}

void MainWin::viewFullScreen(bool on) {
    if (on)
        showFullScreen();
    else
        showNormal();

#ifndef Q_OS_OSX
    acts_.fullScreen->setChecked(on);
#endif
}

void MainWin::viewFiles(bool on) {
    dockFiles_->setVisible(on);
    acts_.viewFiles->setChecked(on);
}

void MainWin::viewDatasets(bool on) {
    dockDatasets_->setVisible(on);
    acts_.viewDatasets->setChecked(on);
}

void MainWin::viewDatasetInfo(bool on) {
    dockDatasetInfo_->setVisible(on);
    acts_.viewDatasetInfo->setChecked(on);
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
