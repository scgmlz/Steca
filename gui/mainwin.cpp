// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_curve.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "mainwin.h"
#include "actions.h"
#include "panels/panel_dataset.h"
#include "panels/panel_diffractogram.h"
#include "panels/panel_fitting.h"
#include "panels/panel_file.h"
#include "io/out_polefigures.h"

#include <QApplication>
#include <QCloseEvent>
#include <QMenuBar>
#include <QStatusBar>
#include <QSplitter>
#include <QMessageBox>
#include <QDate>

#include <QFileDialog>
#include <QDir>
#include <QAction>

#ifdef DEVELOPMENT_REBECCA
#include "io/core_io.h"
#endif

namespace gui {
//------------------------------------------------------------------------------

class SplitImage: public BoxWidget {
  SUPER(SplitImage,BoxWidget)
public:
  SplitImage(TheHub&);
};

SplitImage::SplitImage(TheHub& theHub): super(Qt::Horizontal) {
  auto *options1 = new panel::DatasetOptions1(theHub);
  auto *options2 = new panel::DatasetOptions2(theHub);
  auto *dataset  = new panel::Dataset(theHub);
  connect(options2, &panel::DatasetOptions2::imageScale, dataset, &panel::Dataset::setImageScale);
  _box->addWidget(options1);
  _box->addWidget(options2);
  _box->addWidget(dataset);
  _box->setStretch(2,1);
}

//------------------------------------------------------------------------------

class SplitFitting: public BoxWidget {
  SUPER(SplitFitting,BoxWidget)
public:
  SplitFitting(TheHub&);
};

SplitFitting::SplitFitting(TheHub& theHub): super(Qt::Vertical) {
  _box->addWidget(new panel::Fitting(theHub));
}

//------------------------------------------------------------------------------

class SplitDiffractogram: public BoxWidget {
  SUPER(SplitDiffractogram,BoxWidget)
public:
  SplitDiffractogram(TheHub&);
};

SplitDiffractogram::SplitDiffractogram(TheHub& theHub): super(Qt::Horizontal) {
  auto diffractogram = new panel::Diffractogram(theHub);
  diffractogram->setHorizontalStretch(1);
  _box->addWidget(diffractogram);
}

//------------------------------------------------------------------------------

MainWin::MainWin(): theHub(), actions(theHub.actions) {
  setWindowIcon(QIcon(":/icon/STeCa2"));

  initMenus();
  initLayout();
  initStatus();
  connectActions();

  readSettings();
}

MainWin::~MainWin() {}

void MainWin::initMenus() {
  auto separator = [this]() {
    auto act = new QAction(this);
    act->setSeparator(true);
    return act;
  };

  auto *mbar = menuBar();

  _menuFile     = mbar->addMenu("&File");
  _menuEdit     = mbar->addMenu("&Edit");
  _menuView     = mbar->addMenu("&View");
  _menuDatasets = mbar->addMenu("&Datasets");
  _menuReflect  = mbar->addMenu("&Reflections");
  _menuOutput   = mbar->addMenu("&Output");
  _menuHelp     = mbar->addMenu("&Help");

  _menuFile->addActions({
    actions.addFiles, actions.remFile, separator(),
    actions.loadCorrFile, separator(),
    actions.loadSession, actions.saveSession, actions.loadSession,
  });

  _menuFile->addActions({
#ifndef Q_OS_OSX // Mac puts Quit into the Apple menu
    separator(),
#endif
    actions.quit,
  });

  _menuView->addActions({
    actions.fixedIntensityDisplay, actions.showCut, separator(),
    actions.fitTool, actions.fitBgClear, actions.fitShow, separator(),
    actions.viewStatusbar,
#ifndef Q_OS_OSX
    actions.fullScreen,
#endif
    separator(),
    actions.viewDockFiles,
    actions.viewDockDatasets,
    actions.viewDockDatasetInfo,
    separator(),
    actions.viewReset,
  });

  _menuDatasets->addActions({
    actions.rotateImage, actions.mirrorImage, separator(),
    actions.enableCorr,
  });

  _menuReflect->addActions({
    actions.addReflection, actions.remReflection,
  });

  _menuOutput->addActions({
    actions.outputPolefigures, actions.outputHistograms
  });

  _menuHelp->addActions({
#ifndef Q_OS_OSX // Mac puts About into the Apple menu
    separator(),
#endif
    actions.about,
  });
}

void MainWin::initLayout() {
  addDockWidget(Qt::LeftDockWidgetArea,  (_dockFiles      = new panel::DockFiles(theHub)));
  addDockWidget(Qt::LeftDockWidgetArea,  (_dockDatasets   = new panel::DockDatasets(theHub)));
  addDockWidget(Qt::RightDockWidgetArea, (_dockDatasetInfo  = new panel::DockDatasetInfo(theHub)));

  auto splMain = new QSplitter(Qt::Vertical);
  splMain->setChildrenCollapsible(false);
  setCentralWidget(splMain);

  auto splImages = new QSplitter(Qt::Horizontal);
  splImages->setChildrenCollapsible(false);

  auto splReflections = new QSplitter(Qt::Horizontal);
  splReflections->setChildrenCollapsible(false);

  splMain->addWidget(splImages);
  splMain->addWidget(splReflections);

  splImages->addWidget(new SplitImage(theHub));

  splReflections->addWidget(new SplitFitting(theHub));
  splReflections->addWidget(new SplitDiffractogram(theHub));
  splReflections->setStretchFactor(1, 1);
}

void MainWin::initStatus() {
  statusBar();
}

void MainWin::connectActions() {
  auto onTrigger = [this](QAction* action, void (thisClass::*fun)()) {
    QObject::connect(action, &QAction::triggered, this, fun);
  };

  auto onToggle = [this](QAction* action, void (thisClass::*fun)(bool)) {
    QObject::connect(action, &QAction::toggled, this, fun);
  };

  auto notYet = [this](QAction* action) { action->setEnabled(false); };

  onTrigger(actions.addFiles,     &thisClass::addFiles);
  onTrigger(actions.loadCorrFile, &thisClass::loadCorrFile);

  onTrigger(actions.loadSession,  &thisClass::loadSession);
  onTrigger(actions.saveSession,  &thisClass::saveSession);

  onTrigger(actions.quit,         &thisClass::close);

  onTrigger(actions.outputPolefigures, &thisClass::outputPoleFigures);
  notYet(actions.outputHistograms);

  onTrigger(actions.about, &thisClass::about);

  onToggle(actions.viewStatusbar, &thisClass::viewStatusbar);
#ifndef Q_OS_OSX
  onToggle(actions.fullScreen, &thisClass::viewFullScreen);
#endif

  onToggle(actions.viewDockFiles,       &thisClass::viewDockFiles);
  onToggle(actions.viewDockDatasets,    &thisClass::viewDockDatasets);
  onToggle(actions.viewDockDatasetInfo, &thisClass::viewDockDatasetInfo);

  onTrigger(actions.viewReset, &thisClass::viewReset);
}

void MainWin::about() {
  str appName = qApp->applicationDisplayName();
  str version = qApp->applicationVersion();

  str title = str("About %1").arg(appName);
  str text  = str("<h4>%1 ver. %2</h4>").arg(appName,version);
  str info  = str(
      "StressTexCalculator\n"
      "\n"
      "Copyright: Forschungszentrum Jülich GmbH %1").arg(QDate::currentDate().toString("yyyy"));

  str detailed =
      "Version 1 written by Christian Randau "
      "(Randau, Garbe, Brokmeier, J Appl Cryst 44, 641 (2011)).\n"
      "\n"
      "Version 2 written by Antti Soininen, Jan Burle, Rebecca Brydon.\n";

  auto box = new QMessageBox(this);
  box->setAttribute(Qt::WA_DeleteOnClose);

  box->setWindowTitle(title);
  box->setText(text);
  box->setInformativeText(info);
  box->setDetailedText(detailed);

  auto pm = QPixmap(":/icon/STeCa2").scaled(64,64,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
  box->setIconPixmap(pm);
  box->exec();
}

void MainWin::show() {
  super::show();
  onShow();
}

void MainWin::close() {
  super::close();
}

void MainWin::addFiles() {
  str_lst fileNames = QFileDialog::getOpenFileNames(this, "Add files",
      QDir::current().absolutePath(), "Data files (*.dat);;All files (*.*)");

  if (!fileNames.isEmpty())
    theHub.addFiles(fileNames);
}

void MainWin::loadCorrFile() {
  str fileName = QFileDialog::getOpenFileName(this, "Set correction file",
      QDir::current().absolutePath(), "Data files (*.dat);;All files (*.*)");

  if (!fileName.isEmpty())
    theHub.loadCorrFile(fileName);
}

static str const STE(".ste");

void MainWin::loadSession() {
  str fileName = QFileDialog::getOpenFileName(this, "Load session",
      QDir::current().absolutePath(),
      "Session files (*" % STE % ");;All files (*.*)");

  if (fileName.isEmpty())
    return;

  theHub.load(QFileInfo(fileName));
}

void MainWin::saveSession() {
  str fileName = QFileDialog::getSaveFileName(this, "Save session",
      QDir::current().absolutePath(),
      "Session files (*" % STE % ");;All files (*.*)");

  if (fileName.isEmpty())
    return;

  if (!fileName.endsWith(STE))
    fileName += STE;

  theHub.save(QFileInfo(fileName));
}

void MainWin::outputPoleFigures() {
  auto popup = new io::OutPoleFigures("Pole Figures",theHub,this);
  popup->show();
}

void MainWin::closeEvent(QCloseEvent* event) {
  onClose();
  event->accept();
}

void MainWin::onShow() {
  checkActions();

#ifdef DEVELOPMENT_REBECCA
  theHub.load(QFileInfo("/home/rebecca/SCG/STeCa-Data/1.ste"));
#endif
#ifdef DEVELOPMENT_JAN
#if defined(Q_OS_OSX)
//  theHub.load(QFileInfo("/Users/igb/P/+scg/data/s.ste"));
#else
//  theHub.load(QFileInfo("/home/jan/SCG/s.ste"));
#endif
#endif
}

void MainWin::onClose() {
  saveSettings();
}

static str const GROUP_MAINWIN("MainWin");
static str const KEY_GEOMETRY("geometry");
static str const KEY_STATE("state");

void MainWin::readSettings() {
  if (_initialState.isEmpty())
    _initialState = saveState();

  Settings s(GROUP_MAINWIN);
  restoreGeometry(s.value(KEY_GEOMETRY).toByteArray());
  restoreState(s.value(KEY_STATE).toByteArray());
}

void MainWin::saveSettings() {
  Settings s(GROUP_MAINWIN);
  s.setValue(KEY_GEOMETRY, saveGeometry());
  s.setValue(KEY_STATE,    saveState());
}

void MainWin::checkActions() {
  actions.viewStatusbar->setChecked(statusBar()->isVisible());

#ifndef Q_OS_OSX
  actions.fullScreen->setChecked(isFullScreen());
#endif

  actions.viewDockFiles->setChecked(_dockFiles->isVisible());
  actions.viewDockDatasets->setChecked(_dockDatasets->isVisible());
  actions.viewDockDatasetInfo->setChecked(_dockDatasetInfo->isVisible());
}

void MainWin::viewStatusbar(bool on) {
  statusBar()->setVisible(on);
  actions.viewStatusbar->setChecked(on);
}

void MainWin::viewFullScreen(bool on) {
  if (on)
    showFullScreen();
  else
    showNormal();

#ifndef Q_OS_OSX
  actions.fullScreen->setChecked(on);
#endif
}

void MainWin::viewDockFiles(bool on) {
  _dockFiles->setVisible(on);
  actions.viewDockFiles->setChecked(on);
}

void MainWin::viewDockDatasets(bool on) {
  _dockDatasets->setVisible(on);
  actions.viewDockDatasets->setChecked(on);
}

void MainWin::viewDockDatasetInfo(bool on) {
  _dockDatasetInfo->setVisible(on);
  actions.viewDockDatasetInfo->setChecked(on);
}

void MainWin::viewReset() {
  restoreState(_initialState);
  viewStatusbar(true);
  viewFullScreen(false);
  viewDockFiles(true);
  viewDockDatasets(true);
  viewDockDatasetInfo(true);
}

//------------------------------------------------------------------------------
}
// eof
