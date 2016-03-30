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
#include "mainwin_parts.h"
#include "out/out_polefigures.h"

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

//------------------------------------------------------------------------------

MainWin::MainWin() {
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

  menuFile     = mbar->addMenu("&File");
  menuEdit     = mbar->addMenu("&Edit");
  menuView     = mbar->addMenu("&View");
  menuDatasets = mbar->addMenu("&Datasets");
  menuReflect  = mbar->addMenu("&Reflections");
  menuOutput   = mbar->addMenu("&Output");
  menuHelp     = mbar->addMenu("&Help");

  menuFile->addActions({
      theHub.actAddFiles, theHub.actRemoveFile, separator(),
      theHub.actLoadCorrectionFile, separator(),
      theHub.actLoadSession, theHub.actSaveSession, theHub.actLoadSession,
  });

  QMenu *menuExportDiffractograms = new QMenu("Export diffractograms");
  menuExportDiffractograms->addActions({
      theHub.actExportDiffractogramCurrent,
      theHub.actExportDiffractogramAllSeparateFiles,
      theHub.actExportDiffractogramSingleFile,
  });

  menuFile->addAction(separator());
  menuFile->addMenu(menuExportDiffractograms);

  QMenu *menuExportImages = new QMenu("Export images");
  menuExportImages->addActions({
      theHub.actExportImagesWithMargins, theHub.actExportImagesWithoutMargins,
  });

  menuFile->addMenu(menuExportImages);

  menuFile->addActions({
#ifndef Q_OS_OSX // Mac puts Quit into the Apple menu
      separator(),
#endif
      theHub.actQuit,
  });

  menuEdit->addActions({
      theHub.actUndo, theHub.actRedo, separator(),
      theHub.actCut, theHub.actCopy, theHub.actPaste,
  });

  menuView->addActions({
      theHub.actImagesFixedIntensity, theHub.actImageOverlay, separator(),
      theHub.actBackgroundShowFit, separator(),
      theHub.actViewStatusbar,
#ifndef Q_OS_OSX
      theHub.actFullscreen,
#endif
      separator(),
      theHub.actViewReset,
  });

  menuDatasets->addActions({
      theHub.actImageRotate, theHub.actImageMirror, separator(),
      theHub.actImagesEnableCorr, separator(),
  });

  QMenu *menuNormalization = new QMenu("Normalization");
  menuNormalization->addActions(
      {theHub.actNormalizationDisable, theHub.actNormalizationMeasureTime,
          theHub.actNormalizationMonitor, theHub.actNormalizationBackground});

  menuDatasets->addMenu(menuNormalization);

  menuDatasets->addActions({
      separator(), theHub.actBackgroundBackground, theHub.actBackgroundClear
  });

  menuReflect->addActions({
      theHub.actReflectionAdd, theHub.actReflectionRemove, separator(),
      theHub.actFitErrorParameters
  });

  menuOutput->addActions({
      theHub.actOutputPolefigures, theHub.actOutputHistograms
  });

  menuHelp->addActions({
#ifndef Q_OS_OSX // Mac puts About into the Apple menu
      separator(),
#endif
      theHub.actAbout,
  });
}

void MainWin::initLayout() {
  addDockWidget(Qt::LeftDockWidgetArea,  new panel::DockFiles(theHub));
  addDockWidget(Qt::LeftDockWidgetArea,  new panel::DockDatasets(theHub));
  addDockWidget(Qt::RightDockWidgetArea, new panel::DockDatasetInfo(theHub));

  auto splMain = new QSplitter(Qt::Vertical);
  splMain->setChildrenCollapsible(false);
  setCentralWidget(splMain);

  auto splImages = new QSplitter(Qt::Horizontal);
  splImages->setChildrenCollapsible(false);

  auto splReflections = new QSplitter(Qt::Horizontal);
  splReflections->setChildrenCollapsible(false);

  splMain->addWidget(splImages);
  splMain->addWidget(splReflections);

  splImages->addWidget(new panel::SplitImage(theHub));

  splReflections->addWidget(new panel::SplitFitting(theHub));
  splReflections->addWidget(new panel::SplitDiffractogram(theHub));
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

  onTrigger(theHub.actAddFiles,           &thisClass::addFiles);
  onTrigger(theHub.actLoadCorrectionFile, &thisClass::loadCorrectionFile);

  onTrigger(theHub.actLoadSession,        &thisClass::loadSession);
  onTrigger(theHub.actSaveSession,        &thisClass::saveSession);

  notYet(theHub.actExportDiffractogramCurrent);
  notYet(theHub.actExportDiffractogramAllSeparateFiles);
  notYet(theHub.actExportDiffractogramSingleFile);
  notYet(theHub.actExportImagesWithMargins);
  notYet(theHub.actExportImagesWithoutMargins);

  onTrigger(theHub.actQuit, &thisClass::close);

  notYet(theHub.actUndo);
  notYet(theHub.actRedo);
  notYet(theHub.actCut);
  notYet(theHub.actCopy);
  notYet(theHub.actPaste);

  notYet(theHub.actPreferences);
  notYet(theHub.actFitErrorParameters);

  onTrigger(theHub.actOutputPolefigures, &thisClass::outputPoleFigures);
  notYet(theHub.actOutputHistograms);

  notYet(theHub.actNormalizationDisable);
  notYet(theHub.actNormalizationMeasureTime);
  notYet(theHub.actNormalizationMonitor);
  notYet(theHub.actNormalizationBackground);

  onTrigger(theHub.actAbout, &thisClass::about);

  onToggle(theHub.actViewStatusbar, &thisClass::viewStatusbar);
#ifndef Q_OS_OSX
  onToggle(theHub.actFullscreen, &thisClass::viewFullscreen);
#endif
  onTrigger(theHub.actViewReset, &thisClass::viewReset);
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

void MainWin::loadCorrectionFile() {
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
  auto popup = new OutPoleFigures("Pole Figures",theHub,this);
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
  theHub.load(QFileInfo("/home/jan/SCG/s.ste"));
#endif
}

void MainWin::onClose() {
  saveSettings();
}

static str const GROUP_MAINWIN("MainWin");
static str const KEY_GEOMETRY("geometry");
static str const KEY_STATE("state");

void MainWin::readSettings() {
  if (initialState.isEmpty())
    initialState = saveState();

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
  theHub.actViewStatusbar->setChecked(statusBar()->isVisible());

#ifndef Q_OS_OSX
  theHub.actFullscreen->setChecked(isFullScreen());
#endif
}

void MainWin::viewStatusbar(bool on) {
  statusBar()->setVisible(on);
  theHub.actViewStatusbar->setChecked(on);
}

void MainWin::viewFullscreen(bool on) {
  if (on)
    showFullScreen();
  else
    showNormal();

#ifndef Q_OS_OSX
  theHub.actFullscreen->setChecked(on);
#endif
}

void MainWin::viewReset() {
  restoreState(initialState);
  viewStatusbar(true);
  viewFullscreen(false);
}

//------------------------------------------------------------------------------
// eof
