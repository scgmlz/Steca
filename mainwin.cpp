#include "mainwin.h"
#include "app.h"
#include "action.h"
#include "settings.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include "panes/files.h"

MainWin::MainWin() {
  init();
}

MainWin::~MainWin() {
}

void MainWin::init() {
  initLayout();
  initActionsAndMenus();
  initStatus();
}

#include <QPushButton>
void MainWin::initLayout() {
  typedef QHBoxLayout HL;
  typedef QVBoxLayout VL;
  auto w = new QWidget();
  setCentralWidget(w);
  HL *h = new HL(w);
  VL *v1, *v2, *v3;
  h->addLayout(v1=new VL());
  h->addLayout(v2=new VL());
  h->addLayout(v3=new VL());
  v1->addWidget(new QPushButton("A"));
  v1->addWidget(new QPushButton("B"));
  v1->addWidget(new QPushButton("C"));
  v2->addWidget(new Files());
}

void MainWin::initActionsAndMenus() {
  typedef QKeySequence QKey;

  auto action = [&](rcstr text, bool checkable, bool checked, QKey shortcut) {
    auto act = new QAction(text,this);
    act->setCheckable(checkable);
    act->setChecked(checked);
    act->setShortcut(shortcut);
    return act;
  };

  auto simple = [&](rcstr text, QKey shortcut = QKey::UnknownKey) {
    return action(text,false,false,shortcut);
  };

  auto toggle = [&](rcstr text, bool checked, QKey shortcut = QKey::UnknownKey) {
    return action(text,true,checked,shortcut);
  };

  auto separator = [&]() {
    auto act = new QAction(this);
    act->setSeparator(true);
    return act;
  };

  QMenuBar *mbar = menuBar();

  menuFile = mbar->addMenu("&File");
  menuEdit = mbar->addMenu("&Edit");
  menuView = mbar->addMenu("&View");
  menuOpts = mbar->addMenu("&Options");
  menuHelp = mbar->addMenu("&Help");

  menuFile->addActions({
     actAddFiles             = simple("Add files..."),
     actOpenCorrectionFile   = simple("Open correction file..."),
     separator(),
     actOpenSession          = simple("Open session..."),
     actSaveSession          = simple("Save session..."),
  });

  QMenu *menuExportDiffractograms = new QMenu("Export diffractograms");
  menuExportDiffractograms->addActions({
    actExportDiffractogramCurrent           = simple("Current only..."),
    actExportDiffractogramAllSeparateFiles  = simple("All to separate files..."),
    actExportDiffractogramSingleFile        = simple("All to a single file..."),
  });

  menuFile->addAction(separator());
  menuFile->addMenu(menuExportDiffractograms);

  QMenu *menuExportImages = new QMenu("Export images");
  menuExportImages->addActions({
    actExportImagesWithMargins              = simple("With margins..."),
    actExportImagesWithoutMargins           = simple("Without margins..."),
  });

  menuFile->addMenu(menuExportImages);

  menuFile->addActions({
    separator(),
    actQuit = simple("Quit",QKey::Quit),
  });

  menuEdit->addActions({
    actUndo  = simple("Undo",  QKey::Undo),
    actRedo  = simple("Redo",  QKey::Redo),
    separator(),
    actCut   = simple("Cut",   QKey::Cut),
    actCopy  = simple("Copy",  QKey::Copy),
    actPaste = simple("Paste", QKey::Paste),
  });

  if (!menuBar()->isNativeMenuBar()) {
    menuView->addAction(
      actViewMenubar    = toggle("Menubar",    true, Settings::KEY_VIEW_MENU)
    );
  }

  menuView->addActions({
    actViewStatusbar  = toggle("Statusbar",  true,           Settings::KEY_VIEW_STATUS),
    actFullscreen     = toggle("Fullscreen", isFullScreen(), Settings::KEY_VIEW_FULLSCREEN),
  });

  menuOpts->addActions({
    actPreferences        = simple("Preferences..."),
    actFitErrorParameters = simple("Fit error parameters..."),
  });


  menuHelp->addActions({
    actPdfManual  = simple("Pdf manual (German)"),
    actAbout      = simple("About..."),
  });

  //--- connections ------------------------------------------------------------------

  auto NOT_YET = [](QAction* act) {
    connect(act, &QAction::triggered, [&]() { notYet(); });
  };

  NOT_YET(actAddFiles);
  NOT_YET(actOpenCorrectionFile);

  NOT_YET(actOpenSession);
  NOT_YET(actSaveSession);

  NOT_YET(actExportDiffractogramCurrent);
  NOT_YET(actExportDiffractogramAllSeparateFiles);
  NOT_YET(actExportDiffractogramSingleFile);
  NOT_YET(actExportImagesWithMargins);
  NOT_YET(actExportImagesWithoutMargins);

  NOT_YET(actQuit);

  NOT_YET(actUndo);
  NOT_YET(actRedo);
  NOT_YET(actCut);
  NOT_YET(actCopy);
  NOT_YET(actPaste);

  NOT_YET(actPreferences);
  NOT_YET(actFitErrorParameters);

  NOT_YET(actPdfManual);
  NOT_YET(actAbout);

  connect(actViewMenubar, &QAction::toggled, [&](bool on) {
    menuBar()->setVisible(on);
  });

  connect(actViewStatusbar, &QAction::toggled, [&](bool on) {
    statusBar()->setVisible(on);
  });

  connect(actFullscreen, &QAction::toggled, [&](bool on) {
    if (on) showFullScreen(); else showNormal();
  });
}

void MainWin::initStatus() {
  statusBar();
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

static str const GROUP_MAINWIN("MainWin");
static str const VALUE_GEOMETRY("geometry");
static str const VALUE_STATE("state");

void MainWin::readSettings() {
  if (initialState.isEmpty()) initialState = saveState();
  Settings s;
  s.beginGroup(GROUP_MAINWIN);
  restoreGeometry(s.value(VALUE_GEOMETRY).toByteArray());
  restoreState(s.value(VALUE_STATE).toByteArray());
  s.endGroup();
}

void MainWin::saveSettings() {
  Settings s;
  s.beginGroup(GROUP_MAINWIN);
  s.setValue(VALUE_GEOMETRY,	saveGeometry());
  s.setValue(VALUE_STATE,		saveState());
  s.endGroup();
}

// eof
