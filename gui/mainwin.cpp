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
#include "io/out_sphere.h"

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

SplitImage::SplitImage(TheHub& hub): super(Qt::Horizontal) {
  auto *options1 = new panel::DatasetOptions1(hub);
  auto *options2 = new panel::DatasetOptions2(hub);
  auto *dataset  = new panel::Dataset(hub);
  connect(options2, &panel::DatasetOptions2::imageScale, dataset, &panel::Dataset::setImageScale);
  box_->addWidget(options1);
  box_->addWidget(options2);
  box_->addWidget(dataset);
  box_->setStretch(2,1);
}

//------------------------------------------------------------------------------

class SplitFitting: public BoxWidget {
  SUPER(SplitFitting,BoxWidget)
public:
  SplitFitting(TheHub&);
};

SplitFitting::SplitFitting(TheHub& hub): super(Qt::Vertical) {
  box_->addWidget(new panel::Fitting(hub));
}

//------------------------------------------------------------------------------

class SplitDiffractogram: public BoxWidget {
  SUPER(SplitDiffractogram,BoxWidget)
public:
  SplitDiffractogram(TheHub&);
};

SplitDiffractogram::SplitDiffractogram(TheHub& hub): super(Qt::Horizontal) {
  auto diffractogram = new panel::Diffractogram(hub);
  diffractogram->setHorizontalStretch(1);
  box_->addWidget(diffractogram);
}

//------------------------------------------------------------------------------

MainWin::MainWin(): hub_(), acts_(hub_.actions) {
  setWindowIcon(QIcon(":/icon/STeCa2"));
  QDir::setCurrent(QDir::homePath());

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

  menuFile_     = mbar->addMenu("&File");
  menuEdit_     = mbar->addMenu("&Edit");
  menuView_     = mbar->addMenu("&View");
  menuDatasets_ = mbar->addMenu("&Datasets");
  menuReflect_  = mbar->addMenu("&Reflections");
  menuOutput_   = mbar->addMenu("&Output");
  menuHelp_     = mbar->addMenu("&Help");

#ifdef STECA_LABS
  menuLabs_     = mbar->addMenu("&Labs");
#endif

  menuFile_->addActions({
    acts_.loadSession, acts_.saveSession,
    separator(),
    acts_.addFiles, acts_.remFile,
    separator(),
    acts_.enableCorr, acts_.remCorr,
  });

  menuFile_->addActions({
#ifndef Q_OS_OSX // Mac puts Quit into the Apple menu
    separator(),
#endif
    acts_.quit,
  });

  menuView_->addActions({
    acts_.fixedIntenDisplayImage,acts_.fixedIntenDisplayDgram, acts_.showCut, separator(),
    acts_.fitTool, acts_.fitBgClear, acts_.fitShow, separator(),
    acts_.viewStatusbar,
#ifndef Q_OS_OSX
    acts_.fullScreen,
#endif
    separator(),
    acts_.viewDockFiles,
    acts_.viewDockDatasets,
    acts_.viewDockDatasetInfo,
    separator(),
    acts_.viewReset,
  });

  menuDatasets_->addActions({
    acts_.rotateImage, acts_.mirrorImage, separator(),
    acts_.enableCorr,
  });

  menuReflect_->addActions({
    acts_.addReflection, acts_.remReflection,
  });

  menuOutput_->addActions({
    acts_.outputPolefigures, acts_.outputHistograms
  });

  menuHelp_->addActions({
#ifndef Q_OS_OSX // Mac puts About into the Apple menu
    separator(),
#endif
    acts_.about,
  });

#ifdef STECA_LABS
  menuLabs_->addActions({
    acts_.poleSphere,
  });
#endif
}

void MainWin::initLayout() {
  addDockWidget(Qt::LeftDockWidgetArea,  (dockFiles_      = new panel::DockFiles(hub_)));
  addDockWidget(Qt::LeftDockWidgetArea,  (dockDatasets_   = new panel::DockDatasets(hub_)));
  addDockWidget(Qt::RightDockWidgetArea, (dockDatasetInfo_  = new panel::DockDatasetInfo(hub_)));

  auto splMain = new QSplitter(Qt::Vertical);
  splMain->setChildrenCollapsible(false);
  setCentralWidget(splMain);

  auto splImages = new QSplitter(Qt::Horizontal);
  splImages->setChildrenCollapsible(false);

  auto splReflections = new QSplitter(Qt::Horizontal);
  splReflections->setChildrenCollapsible(false);

  splMain->addWidget(splImages);
  splMain->addWidget(splReflections);

  splImages->addWidget(new SplitImage(hub_));

  splReflections->addWidget(new SplitFitting(hub_));
  splReflections->addWidget(new SplitDiffractogram(hub_));
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

  onTrigger(acts_.loadSession,  &thisClass::loadSession);
  onTrigger(acts_.saveSession,  &thisClass::saveSession);

  onTrigger(acts_.addFiles,     &thisClass::addFiles);
  onTrigger(acts_.enableCorr,   &thisClass::enableCorr);

  onTrigger(acts_.quit,         &thisClass::close);

  onTrigger(acts_.outputPolefigures, &thisClass::outputPoleFigures);
  notYet(acts_.outputHistograms);

#ifdef STECA_LABS
  onTrigger(acts_.poleSphere, &thisClass::poleSphere);
#endif

  onTrigger(acts_.about, &thisClass::about);

  onToggle(acts_.viewStatusbar, &thisClass::viewStatusbar);
#ifndef Q_OS_OSX
  onToggle(acts_.fullScreen, &thisClass::viewFullScreen);
#endif

  onToggle(acts_.viewDockFiles,       &thisClass::viewDockFiles);
  onToggle(acts_.viewDockDatasets,    &thisClass::viewDockDatasets);
  onToggle(acts_.viewDockDatasetInfo, &thisClass::viewDockDatasetInfo);

  onTrigger(acts_.viewReset, &thisClass::viewReset);
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

  if (!fileNames.isEmpty()) {
    hub_.addFiles(fileNames);
    QDir::setCurrent(QFileInfo(fileNames.at(0)).absolutePath());
  }
}

void MainWin::enableCorr() {
  str fileName;
  if (!hub_.hasCorrFile())
    fileName = QFileDialog::getOpenFileName(this, "Set correction file",
      QDir::current().absolutePath(), "Data files (*.dat);;All files (*.*)");

  if (!fileName.isEmpty()) {
    hub_.setCorrFile(fileName);
    QDir::setCurrent(QFileInfo(fileName).absolutePath());
  }
}

static str const STE(".ste");

void MainWin::loadSession() {
  str fileName = QFileDialog::getOpenFileName(this, "Load session",
      QDir::current().absolutePath(),
      "Session files (*" % STE % ");;All files (*.*)");

  if (fileName.isEmpty())
    return;

  hub_.loadSession(QFileInfo(fileName));
}

void MainWin::saveSession() {
  str fileName = QFileDialog::getSaveFileName(this, "Save session",
      QDir::current().absolutePath(),
      "Session files (*" % STE % ");;All files (*.*)");

  if (fileName.isEmpty())
    return;

  if (!fileName.endsWith(STE))
    fileName += STE;

  hub_.saveSession(QFileInfo(fileName));
}

void MainWin::outputPoleFigures() {
  auto popup = new io::OutPoleFigures(hub_,"Pole Figures",this);
  popup->show();
}

#ifdef STECA_LABS
void MainWin::poleSphere() {
  auto popup = new io::PoleSphere(hub_,"Pole Sphere",this);
  popup->show();
}
#endif

void MainWin::closeEvent(QCloseEvent* event) {
  onClose();
  event->accept();
}

void MainWin::onShow() {
  checkActions();

#ifdef DEVELOPMENT_REBECCA
  hub_.load(QFileInfo("/home/rebecca/SCG/STeCa-Data/1.ste"));
#endif
#ifdef DEVELOPMENT_JAN
#if defined(Q_OS_OSX)
//  hub_.load(QFileInfo("/Users/igb/P/+scg/data/s.ste"));
#else
  hub_.loadSession(QFileInfo("/home/jan/SCG/s.ste"));
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
  if (initialState_.isEmpty())
    initialState_ = saveState();

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
  acts_.viewStatusbar->setChecked(statusBar()->isVisible());

#ifndef Q_OS_OSX
  acts_.fullScreen->setChecked(isFullScreen());
#endif

  acts_.viewDockFiles->setChecked(dockFiles_->isVisible());
  acts_.viewDockDatasets->setChecked(dockDatasets_->isVisible());
  acts_.viewDockDatasetInfo->setChecked(dockDatasetInfo_->isVisible());
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

void MainWin::viewDockFiles(bool on) {
  dockFiles_->setVisible(on);
  acts_.viewDockFiles->setChecked(on);
}

void MainWin::viewDockDatasets(bool on) {
  dockDatasets_->setVisible(on);
  acts_.viewDockDatasets->setChecked(on);
}

void MainWin::viewDockDatasetInfo(bool on) {
  dockDatasetInfo_->setVisible(on);
  acts_.viewDockDatasetInfo->setChecked(on);
}

void MainWin::viewReset() {
  restoreState(initialState_);
  viewStatusbar(true);
  viewFullScreen(false);
  viewDockFiles(true);
  viewDockDatasets(true);
  viewDockDatasetInfo(true);
}

//------------------------------------------------------------------------------
}
// eof
