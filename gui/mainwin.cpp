#include "mainwin.h"
#include "mainwin_parts.h"

#include <QCloseEvent>
#include <QMenuBar>
#include <QStatusBar>
#include <QSplitter>

#include <QFileDialog>
#include <QDir>
#include <QAction>

#ifdef DEVELOPMENT_REBECCA
#include "io/core_io.h"
#endif
//------------------------------------------------------------------------------

MainWin::MainWin() {
  sessionDir = dataDir = QDir::homePath();
  setWindowIcon(QIcon(":/icon/STeCa2"));

  initMenus();
  initLayout();
  initStatus();
  connectActions();

  readSettings();

}

MainWin::~MainWin() {
}

void MainWin::initMenus() {
  auto separator = [this]() {
    auto act = new QAction(this);
    act->setSeparator(true);
    return act;
  };

  auto *mbar = menuBar();

  menuFile  = mbar->addMenu("&File");
  menuEdit  = mbar->addMenu("&Edit");
  menuView  = mbar->addMenu("&View");
  menuDatasets = mbar->addMenu("&Datasets");
  menuReflect  = mbar->addMenu("&Reflections");
  menuHelp  = mbar->addMenu("&Help");

  menuFile->addActions({
    theHub.actAddFiles, theHub.actRemoveFile,
    separator(),
    theHub.actLoadCorrectionFile,
    separator(),
    theHub.actLoadSession, theHub.actSaveSession, theHub.actLoadSession
  });

  QMenu *menuExportDiffractograms = new QMenu("Export diffractograms",this);
  menuExportDiffractograms->addActions({
    theHub.actExportDiffractogramCurrent,
    theHub.actExportDiffractogramAllSeparateFiles,
    theHub.actExportDiffractogramSingleFile,
  });

  menuFile->addAction(separator());
  menuFile->addMenu(menuExportDiffractograms);

  QMenu *menuExportImages = new QMenu("Export images",this);
  menuExportImages->addActions({
    theHub.actExportImagesWithMargins,
    theHub.actExportImagesWithoutMargins,
  });

  menuFile->addMenu(menuExportImages);

  menuFile->addActions({
  #ifndef Q_OS_OSX  // Mac puts Quit into the Apple menu
    separator(),
  #endif
    theHub.actQuit,
  });

  menuEdit->addActions({
    theHub.actUndo, theHub.actRedo,
    separator(),
    theHub.actCut, theHub.actCopy, theHub.actPaste,
  });

  menuView->addActions({
    theHub.actImagesFixedIntensity,
    theHub.actImageOverlay,
    separator(),
    theHub.actBackgroundShowFit,
    separator(),
    theHub.actViewStatusbar,
  #ifndef Q_OS_OSX
    theHub.actFullscreen,
  #endif
    separator(),
    theHub.actViewReset,
  });

  menuDatasets->addActions({
    theHub.actImageRotate, theHub.actImageMirror,
    separator(),
    theHub.actImagesEnableCorr,
    separator(),
  });

  QMenu *menuNormalization = new QMenu("Normalizaiton",this);
  menuNormalization->addActions({
    theHub.actNormalizationDisable, theHub.actNormalizationMeasureTime,
    theHub.actNormalizationMonitor, theHub.actNormalizationBackground
  });

  menuDatasets->addMenu(menuNormalization);

  menuDatasets->addActions({
    separator(),
    theHub.actBackgroundBackground,
    theHub.actBackgroundClear
  });

  menuReflect->addActions({
    theHub.actReflectionAdd, theHub.actReflectionRemove, theHub.actReflectionSelectRegion,
    separator(),
    theHub.actCalculatePolefigures, theHub.actCalculateHistograms,
    separator(),
    theHub.actFitErrorParameters
  });

  menuHelp->addActions({
  #ifndef Q_OS_OSX // Mac puts About into the Apple menu
    separator(),
  #endif
    theHub.actAbout,
  });
}

void MainWin::initLayout() {
  addDockWidget(Qt::LeftDockWidgetArea, new panel::DockFiles(theHub));
  addDockWidget(Qt::LeftDockWidgetArea, new panel::DockDatasets(theHub));
  addDockWidget(Qt::RightDockWidgetArea,new panel::DockDatasetInfo(theHub));

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
  splReflections->setStretchFactor(1,1);
}

void MainWin::initStatus() {
  statusBar();
}

void MainWin::connectActions() {
  auto onTrigger = [this](QAction* action, void (thisCls::*fun)()) {
    QObject::connect(action, &QAction::triggered, this, fun);
  };

  auto onToggle = [this](QAction* action, void (thisCls::*fun)(bool)) {
    QObject::connect(action, &QAction::toggled, this, fun);
  };

  auto notYet = [this](QAction* action) {
    action->setEnabled(false);
  };

  onTrigger(theHub.actAddFiles,            &thisCls::addFiles);
  onTrigger(theHub.actLoadCorrectionFile,  &thisCls::loadCorrectionFile);

  onTrigger(theHub.actLoadSession, &thisCls::loadSession);
  onTrigger(theHub.actSaveSession, &thisCls::saveSession);

  notYet(theHub.actExportDiffractogramCurrent);
  notYet(theHub.actExportDiffractogramAllSeparateFiles);
  notYet(theHub.actExportDiffractogramSingleFile);
  notYet(theHub.actExportImagesWithMargins);
  notYet(theHub.actExportImagesWithoutMargins);

  onTrigger(theHub.actQuit, &thisCls::close);

  notYet(theHub.actUndo);
  notYet(theHub.actRedo);
  notYet(theHub.actCut);
  notYet(theHub.actCopy);
  notYet(theHub.actPaste);

  notYet(theHub.actPreferences);
  notYet(theHub.actReflectionSelectRegion);
  notYet(theHub.actFitErrorParameters);

  notYet(theHub.actCalculatePolefigures);
  notYet(theHub.actCalculateHistograms);

  notYet(theHub.actNormalizationDisable);
  notYet(theHub.actNormalizationMeasureTime);
  notYet(theHub.actNormalizationMonitor);
  notYet(theHub.actNormalizationBackground);

  notYet(theHub.actAbout);

  onToggle(theHub.actViewStatusbar, &thisCls::viewStatusbar);
#ifndef Q_OS_OSX
  onToggle(theHub.actFullscreen,    &thisCls::viewFullscreen);
#endif
  onTrigger(theHub.actViewReset,    &thisCls::viewReset);
}

void MainWin::show() {
  super::show();
  checkActions();

#ifdef DEVELOPMENT_JAN
#ifdef Q_OS_OSX
  theHub.load(QFileInfo("/Users/igb/P/+scg/STeCa/data/q.ste"));
#else
  theHub.load(QFileInfo("/home/jan/q.ste"));
#endif
#endif

#ifdef DEVELOPMENT_REBECCA
  theHub.load(QFileInfo("/home/rebecca/SCG/STeCa-Data/1.ste"));
#endif
}

void MainWin::close() {
  super::close();
}

void MainWin::addFiles() {
  str_lst fileNames = QFileDialog::getOpenFileNames(this,
    "Add files", dataDir, "Data files (*.dat);;All files (*.*)");

  if (!fileNames.isEmpty()) {
    // remember the directory for the next time
    dataDir = QFileInfo(fileNames.first()).absolutePath();
    theHub.addFiles(fileNames);
  }
}

void MainWin::loadCorrectionFile() {
  str fileName = QFileDialog::getOpenFileName(this,
      "Set correction file", dataDir, "Data files (*.dat);;All files (*.*)");

  if (!fileName.isEmpty()) {
    // remember the directory for the next time
    dataDir = QFileInfo(fileName).absolutePath();
    theHub.loadCorrFile(fileName);
  }
}

static str const STE(".ste");

void MainWin::loadSession() {
  str fileName = QFileDialog::getOpenFileName(this,
      "Load session", sessionDir, "Session files (*"%STE%");;All files (*.*)");
  if (fileName.isEmpty()) return;

  QFileInfo fileInfo(fileName);
  sessionDir = fileInfo.absolutePath();
  theHub.load(fileInfo);
}

void MainWin::saveSession() {
  str fileName = QFileDialog::getSaveFileName(this,
      "Save session", sessionDir, "Session files (*"%STE%");;All files (*.*)");
  if (fileName.isEmpty()) return;

  if (!fileName.endsWith(STE)) fileName += STE;
  QFileInfo info(fileName);
  sessionDir = info.absolutePath();

  QFile file(info.filePath());
  RUNTIME_CHECK(file.open(QIODevice::WriteOnly), "File cannot be opened");

  auto written = file.write(theHub.save());
  RUNTIME_CHECK(written >= 0, "Could not write session");
}

void MainWin::closeEvent(QCloseEvent* event) {
  onClose();
  event->accept();
}

void MainWin::onClose() {
  saveSettings();
}

static str GROUP_MAINWIN("MainWin");
static str KEY_GEOMETRY("geometry");
static str KEY_STATE("state");
static str KEY_DATADIR("dataDir");
static str KEY_SESSION_DIR("sessionDir");

void MainWin::readSettings() {
  if (initialState.isEmpty()) initialState = saveState();
  Settings s(GROUP_MAINWIN);
  restoreGeometry(s.value(KEY_GEOMETRY).toByteArray());
  restoreState(s.value(KEY_STATE).toByteArray());

  QDir savedDataDir(s.value(KEY_DATADIR).toString());
  if (savedDataDir.exists()) dataDir = savedDataDir.absolutePath();
  QDir savedSessionDir(s.value(KEY_SESSION_DIR).toString());
  if (savedSessionDir.exists()) sessionDir = savedSessionDir.absolutePath();
}

void MainWin::saveSettings() {
  Settings s(GROUP_MAINWIN);
  s.setValue(KEY_GEOMETRY, saveGeometry());
  s.setValue(KEY_STATE,    saveState());
  s.setValue(KEY_DATADIR,    dataDir);
  s.setValue(KEY_SESSION_DIR, sessionDir);
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
  if (on) showFullScreen(); else showNormal();
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
