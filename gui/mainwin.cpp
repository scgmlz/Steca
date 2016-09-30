// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      core_curve.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "mainwin.h"
#include "../manifest.h"
#include "about.h"
#include "app.h"
#include "output/output_diagrams.h"
#include "output/output_diffractograms.h"
#include "output/output_polefigures.h"
#include "panels/dock_dataset.h"
#include "panels/dock_files.h"
#include "panels/dock_metadata.h"
#include "panels/tabs_diffractogram.h"
#include "panels/tabs_images.h"
#include "panels/tabs_setup.h"

#include <QCloseEvent>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSplitter>
#include <QStatusBar>
#include <QUrl>

namespace gui {
//------------------------------------------------------------------------------

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
  mbar->setNativeMenuBar(true);

  menuFile_     = mbar->addMenu("&File");
  menuView_     = mbar->addMenu("&View");
  menuImage_    = mbar->addMenu("&Image");
  menuDgram_    = mbar->addMenu("Di&ffractogram");
  menuOutput_   = mbar->addMenu("&Output");
  menuHelp_     = mbar->addMenu("&Help");

  menuFile_->addActions({
      acts_.addFiles, acts_.remFile,
      separator(),
      acts_.enableCorr, acts_.remCorr,
      separator(),
      acts_.loadSession, acts_.saveSession,
  });

  menuFile_->addActions({
    #ifndef Q_OS_OSX  // Mac puts Quit into the Apple menu
      separator(),
    #endif
      acts_.quit,
  });

  menuView_->addActions({
      acts_.viewFiles, acts_.viewDatasets, acts_.viewDatasetInfo,
      separator(),
    #ifndef Q_OS_OSX
      acts_.fullScreen,
    #endif
      acts_.viewStatusbar,
      separator(),
      acts_.viewReset,
  });

  menuImage_->addActions({
      acts_.rotateImage,
      acts_.mirrorImage,
      acts_.fixedIntenImageScale,
      acts_.showOverlay,
      acts_.stepScale,
      acts_.linkCuts,
  });

  menuDgram_->addActions({
      acts_.selRegions,
      acts_.showBackground,
      acts_.clearBackground,
      acts_.clearReflections,
      separator(),
      acts_.addReflection, acts_.remReflection,
      separator(),
      acts_.combinedDgram,
      acts_.fixedIntenDgramScale,
  });

  menuOutput_->addActions({
      acts_.outputPolefigures, acts_.outputDiagrams, acts_.outputDiffractograms,
  });

  menuHelp_->addActions({
    acts_.about,
  #ifndef Q_OS_OSX
    separator(),  // Mac puts About into the Apple menu
  #endif
    acts_.online,
    acts_.checkUpdate,
  });
}

void MainWin::initLayout() {
  addDockWidget(Qt::LeftDockWidgetArea, (dockFiles_       = new panel::DockFiles(hub_)));
  addDockWidget(Qt::LeftDockWidgetArea, (dockDatasets_    = new panel::DockDatasets(hub_)));
  addDockWidget(Qt::RightDockWidgetArea,(dockDatasetInfo_ = new panel::DockMetadata(hub_)));

  auto splMain = new QSplitter(Qt::Vertical);
  splMain->setChildrenCollapsible(false);

  auto splTop = new QSplitter(Qt::Horizontal);
  splTop->setChildrenCollapsible(false);

  setCentralWidget(splMain);

  splMain->addWidget(splTop);
  splMain->addWidget(new panel::TabsDiffractogram(hub_));

  splTop->addWidget(new panel::TabsSetup(hub_));
  splTop->addWidget(new panel::TabsImages(hub_));
  splTop->setStretchFactor(1,1);
}

void MainWin::initStatusBar() {
  statusBar();
}

void MainWin::connectActions() {
  auto onTrigger = [this](QAction* action, void (Cls::*fun)()) {
    QObject::connect(action, &QAction::triggered, this, fun);
  };

  auto onToggle = [this](QAction* action, void (Cls::*fun)(bool)) {
    QObject::connect(action, &QAction::toggled, this, fun);
  };

  onTrigger(acts_.loadSession, &Cls::loadSession);
  onTrigger(acts_.saveSession, &Cls::saveSession);

  onTrigger(acts_.addFiles, &Cls::addFiles);
  onTrigger(acts_.enableCorr, &Cls::enableCorr);

  onTrigger(acts_.quit, &Cls::close);

  onTrigger(acts_.outputPolefigures,    &Cls::outputPoleFigures);
  onTrigger(acts_.outputDiagrams,       &Cls::outputDiagrams);
  onTrigger(acts_.outputDiffractograms, &Cls::outputDiffractograms);

  onTrigger(acts_.about,       &Cls::about);
  onTrigger(acts_.online,      &Cls::online);
  onTrigger(acts_.checkUpdate, &Cls::checkUpdate);

  onToggle(acts_.viewStatusbar, &Cls::viewStatusbar);
#ifndef Q_OS_OSX
  onToggle(acts_.fullScreen, &Cls::viewFullScreen);
#endif

  onToggle(acts_.viewFiles, &Cls::viewFiles);
  onToggle(acts_.viewDatasets, &Cls::viewDatasets);
  onToggle(acts_.viewDatasetInfo, &Cls::viewDatasetInfo);

  onTrigger(acts_.viewReset, &Cls::viewReset);
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
  NoWarnings _;

  QNetworkRequest req;

  req.setUrl(QUrl(STECA2_VERSION_URL));
  auto reply = netMan_.get(req);

  connect(reply, &QNetworkReply::finished, [this, completeReport, reply]() {
    auto strLst = str(reply->readAll()).split("\n");
    reply->deleteLater();

    str lastVer;
    if (!strLst.isEmpty()) {
      lastVer = strLst.first();
      lastVer = lastVer.mid(1, lastVer.length()-2);
    }

    str ver  = qApp->applicationVersion();
    str name = qApp->applicationName();

    if (ver != lastVer)
      messageDialog(
        str("%1 update").arg(name),
        str("<p>The latest %1 version is %2. You have %3.</p>"
            "<p><a href='%4'>get new version</a></p>")
            .arg(name, lastVer, ver, STECA2_DOWNLOAD_URL));
    else if (completeReport)
      messageDialog(
        str("%1 update").arg(name),
        str("<p>You have the latest %1 version (%2).</p>")
            .arg(name).arg(ver));
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
  str_lst fileNames = QFileDialog::getOpenFileNames(
      this, "Add files", QDir::current().absolutePath(),
      "Data files (*.dat);;All files (*.*)");

  if (!fileNames.isEmpty()) {
    QDir::setCurrent(QFileInfo(fileNames.at(0)).absolutePath());
    hub_.addFiles(fileNames);
  }
}

void MainWin::enableCorr() {
  str fileName;
  if (!hub_.hasCorrFile())
    fileName = QFileDialog::getOpenFileName(
        this, "Set correction file", QDir::current().absolutePath(),
        "Data files (*.dat);;All files (*.*)");

  if (!fileName.isEmpty()) {
    QDir::setCurrent(QFileInfo(fileName).absolutePath());
    hub_.setCorrFile(fileName);
  }
}

static str const STE(".ste");

void MainWin::loadSession() {
  str fileName = QFileDialog::getOpenFileName(
      this, "Load session", QDir::current().absolutePath(),
      "Session files (*" % STE % ");;All files (*.*)");

  if (fileName.isEmpty())
    return;

  hub_.loadSession(QFileInfo(fileName));
}

void MainWin::saveSession() {
  QFileDialog dlg(this, "Save session", QDir::current().absolutePath(),
                  "Session files (*" % STE % ");;All files (*.*)");
  dlg.setFileMode(QFileDialog::AnyFile);
  dlg.setAcceptMode(QFileDialog::AcceptSave);
  dlg.setConfirmOverwrite(false);

  if (!dlg.exec())
    return;

  auto files = dlg.selectedFiles();
  if (files.isEmpty())
    return;

  str fileName = files.first();
  if (!fileName.endsWith(STE)) fileName += STE;

  hub_.saveSession(QFileInfo(fileName));
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

#ifdef DEVELOPMENT_JAN
  auto safeLoad = [this](rcstr fileName) {
    QFileInfo info(QDir::homePath() % fileName);
    if (info.exists())
      hub_.loadSession(info);
  };
#endif

#ifdef DEVELOPMENT_JAN
  safeLoad("/C/+scg/data/0.ste");
//  hub_.actions.outputDiagrams->trigger();
#endif

  Settings s(GROUP_CONFIG);
  if (s.readBool(KEY_STARTUP_CHECK_UPDATE, true))
    checkUpdate(false);
  if (s.readBool(KEY_STARTUP_ABOUT, true))
    about();
}

void MainWin::onClose() {
  saveSettings();
}

static str const GROUP_MAINWIN("MainWin");
static str const KEY_GEOMETRY("geometry");
static str const KEY_STATE("state");

void MainWin::readSettings() {
  if (initialState_.isEmpty()) initialState_ = saveState();

  Settings s(GROUP_MAINWIN);
  restoreGeometry(s.value(KEY_GEOMETRY).toByteArray());
  restoreState(s.value(KEY_STATE).toByteArray());
}

void MainWin::saveSettings() {
  Settings s(GROUP_MAINWIN);
  s.setValue(KEY_GEOMETRY, saveGeometry());
  s.setValue(KEY_STATE, saveState());
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

//------------------------------------------------------------------------------
}
// eof
