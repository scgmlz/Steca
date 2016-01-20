#include "mainwin.h"
#include "app.h"
#include "settings.h"
#include "split_files.h"
#include "split_image.h"
#include "split_datasets.h"
#include "split_reflections.h"
#include "split_diffractogram.h"

#include <QCloseEvent>
#include <QMenuBar>
#include <QStatusBar>
#include <QSplitter>

#include <QFileDialog>

MainWin::MainWin() {
  sessionDir = dataDir = QDir::homePath();

  initActions();
  initMenus();
  initLayout();
  initStatus();
  connectActions();

  readSettings();
}

MainWin::~MainWin() {
}

void MainWin::initActions() {
  typedef QKeySequence QKey;

  auto action = [this](pcstr text, bool checkable, pcstr iconFile, QKey shortcut) {
    ASSERT(text)
    pcstr tip = text;
    auto act = new QAction(text,this);
    act->setToolTip(tip);
    act->setCheckable(checkable);
    if (iconFile && *iconFile) act->setIcon(QIcon(iconFile));
    act->setShortcut(shortcut);
    return act;
  };

  auto simple = [action](pcstr text, pcstr iconFile = nullptr, QKey shortcut = QKey::UnknownKey) {
    ASSERT(text)
    return action(text,false,iconFile,shortcut);
  };

  auto toggle = [action](pcstr text, pcstr iconFile = nullptr, QKey shortcut = QKey::UnknownKey) {
    return action(text,true,iconFile,shortcut);
  };

  Keys keys;

  actAddFiles           = simple("Add files...",          ":/icon/add", keys.keyAddFiles);
  actRemoveFile         = simple("Remove selected file",  ":/icon/rem", keys.keyDeleteFile);
  actSetCorrectionFile  = simple("Set correction file...","",           keys.keySetCorrectionFile);
  actOpenSession        = simple("Open session...");
  actSaveSession        = simple("Save session...");

  actExportDiffractogramCurrent           = simple("Current only...");
  actExportDiffractogramAllSeparateFiles  = simple("All to separate files...");
  actExportDiffractogramSingleFile        = simple("All to a single file...");

  actExportImagesWithMargins              = simple("With margins...");
  actExportImagesWithoutMargins           = simple("Without margins...");

  actQuit  = simple("Quit",  "", QKey::Quit);

  actUndo  = simple("Undo",  "", QKey::Undo);
  actRedo  = simple("Redo",  "", QKey::Redo);
  actCut   = simple("Cut",   "", QKey::Cut);
  actCopy  = simple("Copy",  "", QKey::Copy);
  actPaste = simple("Paste", "", QKey::Paste);

  actViewStatusbar = toggle("Statusbar", "", keys.keyViewStatusbar);
#ifndef Q_OS_OSX
  actFullscreen    = toggle("Fullscreen", "", keys.keyFullscreen);
#endif
  actViewReset     = simple("Reset");

  actPreferences        = simple("Preferences...");
  actFitErrorParameters = simple("Fit error parameters...");

  actPdfManual  = simple("Pdf manual (German)");
  actAbout      = simple("About...");

  actReflectionPeak     = simple("Peak",      ":/icon/peak");
  actReflectionReflect  = simple("Reflect",   ":/icon/reflect");
  actReflectionWidth    = simple("Width",     ":/icon/width");
  actReflectionAdd      = simple("Width",     ":/icon/add");

  actImagesLink         = toggle("Link",      ":/icon/link");
  actImagesEye          = toggle("eye",       ":/icon/eye");
  actImagesGlobalNorm   = toggle("global nm.");
  actImagesUpDown       = toggle("UpDown",    ":/icon/updown");
  actImagesLeftRight    = toggle("LeftRight", ":/icon/leftright");
  actImagesTurnRight    = toggle("TurnRight", ":/icon/turnright");
  actImagesTurnLeft     = toggle("TurnLeft",  ":/icon/turnleft");

  actBackgroundBackground = simple("Background",    ":/icon/background");
  actBackgroundEye        = simple("BackgroundEye", ":/icon/eye");
}

void MainWin::initMenus() {
  auto separator = [this]() {
    auto act = new QAction(this);
    act->setSeparator(true);
    return act;
  };

  auto *mbar = menuBar();

  menuFile = mbar->addMenu("&File");
  menuEdit = mbar->addMenu("&Edit");
  menuView = mbar->addMenu("&View");
  menuOpts = mbar->addMenu("&Options");
  menuHelp = mbar->addMenu("&Help");

  menuFile->addActions({
    actAddFiles, actRemoveFile,
    separator(),
    actSetCorrectionFile,
    separator(),
    actOpenSession, actSaveSession,actOpenSession
  });

  QMenu *menuExportDiffractograms = new QMenu("Export diffractograms",this);
  menuExportDiffractograms->addActions({
    actExportDiffractogramCurrent,
    actExportDiffractogramAllSeparateFiles,
    actExportDiffractogramSingleFile,
  });

  menuFile->addAction(separator());
  menuFile->addMenu(menuExportDiffractograms);

  QMenu *menuExportImages = new QMenu("Export images",this);
  menuExportImages->addActions({
    actExportImagesWithMargins,
    actExportImagesWithoutMargins,
  });

  menuFile->addMenu(menuExportImages);

  menuFile->addActions({
  #ifndef Q_OS_OSX  // Mac puts Quit into the Apple menu
    separator(),
  #endif
    actQuit,
  });

  menuEdit->addActions({
    actUndo, actRedo,
    separator(),
    actCut, actCopy, actPaste,
  });

  menuView->addActions({
    actViewStatusbar,
  #ifndef Q_OS_OSX
    actFullscreen,
  #endif
    separator(),
    actViewReset,
  });

  menuOpts->addActions({
    actPreferences, actFitErrorParameters,
  });

  menuHelp->addActions({
    actPdfManual,
  #ifndef Q_OS_OSX // Mac puts About into the Apple menu
    separator(),
  #endif
    actAbout,
  });
}

void MainWin::initLayout() {
  auto splMain = new QSplitter(Qt::Horizontal);
  splMain->setChildrenCollapsible(false);

  setCentralWidget(splMain);

  auto splOther = new QSplitter(Qt::Vertical);
  splOther->setChildrenCollapsible(false);

  auto splImages = new QSplitter(Qt::Horizontal);
  splImages->setChildrenCollapsible(false);

  auto splReflections = new QSplitter(Qt::Horizontal);
  splReflections->setChildrenCollapsible(false);

  splMain->addWidget((splitFiles = new SplitFiles(*this)));
  splMain->addWidget(splOther);
  splMain->setSizes({0,INT16_MAX});

  splOther->addWidget(splImages);
  splOther->addWidget(splReflections);

  splImages->addWidget((splitDatasets = new SplitDatasets(*this)));
  splImages->addWidget((splitImage = new SplitImage(*this)));

  splReflections->addWidget((splitReflections = new SplitReflections(*this)));
  splReflections->addWidget((splitDiffractogram = new SplitDiffractogram(*this)));
  splReflections->setStretchFactor(0,1);
  splReflections->setStretchFactor(1,3);
}

void MainWin::initStatus() {
  statusBar();
}

void MainWin::connectActions() {
  auto onTrigger = [this](QAction* action, void (MainWin::*fun)()) {
    QObject::connect(action, &QAction::triggered, this, fun);
  };

  auto onToggle = [this](QAction* action, void (MainWin::*fun)(bool)) {
    QObject::connect(action, &QAction::toggled, this, fun);
  };

  auto NOT_YET = [this](QAction* action) {
    action->setEnabled(false);
  };

  onTrigger(actAddFiles, &MainWin::addFiles);
  actRemoveFile->setEnabled(false);
  connect(&session, &Session::fileSelected, this, [this](pcCoreFile file) {
    actRemoveFile->setEnabled(nullptr!=file);
  });

  onTrigger(actSetCorrectionFile,  &MainWin::setCorrectionFile);

  onTrigger(actOpenSession, &MainWin::loadSession);
  onTrigger(actSaveSession, &MainWin::saveSession);

  NOT_YET(actExportDiffractogramCurrent);
  NOT_YET(actExportDiffractogramAllSeparateFiles);
  NOT_YET(actExportDiffractogramSingleFile);
  NOT_YET(actExportImagesWithMargins);
  NOT_YET(actExportImagesWithoutMargins);

  onTrigger(actQuit, &MainWin::close);

  NOT_YET(actUndo);
  NOT_YET(actRedo);
  NOT_YET(actCut);
  NOT_YET(actCopy);
  NOT_YET(actPaste);

  NOT_YET(actPreferences);
  NOT_YET(actFitErrorParameters);

  NOT_YET(actPdfManual);
  NOT_YET(actAbout);

  onToggle(actViewStatusbar, &MainWin::viewStatusbar);
#ifndef Q_OS_OSX
  onToggle(actFullscreen,    &MainWin::viewFullscreen);
#endif
  onTrigger(actViewReset,    &MainWin::viewReset);
}

void MainWin::show() {
  super::show();
  checkActions();
#ifdef DEVEL
  session.load(QFileInfo("/home/jan/q.ste"));
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
    session.addFiles(fileNames);
  }
}

void MainWin::setCorrectionFile() {
  str fileName = QFileDialog::getOpenFileName(this,
      "Set correction file", dataDir, "Data files (*.dat);;All files (*.*)");

  if (!fileName.isEmpty()) {
    // remember the directory for the next time
    dataDir = QFileInfo(fileName).absolutePath();
    session.setCorrFile(fileName);
  }
}

static str const STE(".ste");

void MainWin::loadSession() {
  str fileName = QFileDialog::getOpenFileName(this,
      "Load session", sessionDir, "Session files (*"%STE%");;All files (*.*)");
  if (fileName.isEmpty()) return;

  QFileInfo fileInfo(fileName);
  sessionDir = fileInfo.absolutePath();
  session.load(fileInfo);
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

  auto written = file.write(session.save());
  RUNTIME_CHECK(written >= 0, "Could not write session");
}

void MainWin::closeEvent(QCloseEvent* event) {
  if (onClose())
    event->accept();
  else
    event->ignore();
}

bool MainWin::onClose() {
  saveSettings();
  return true;
}

static str GROUP_MAINWIN("MainWin");
static str KEY_GEOMETRY("geometry");
static str KEY_STATE("state");

void MainWin::readSettings() {
  if (initialState.isEmpty()) initialState = saveState();
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
  actViewStatusbar->setChecked(statusBar()->isVisible());
#ifndef Q_OS_OSX
  actFullscreen->setChecked(isFullScreen());
#endif
}

void MainWin::viewStatusbar(bool on) {
  statusBar()->setVisible(on);
  actViewStatusbar->setChecked(on);
}

void MainWin::viewFullscreen(bool on) {
  if (on) showFullScreen(); else showNormal();
#ifndef Q_OS_OSX
  actFullscreen->setChecked(on);
#endif
}

void MainWin::viewReset() {
  restoreState(initialState);
  viewStatusbar(true);
  viewFullscreen(false);
}

// eof
