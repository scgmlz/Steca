#include "mainwin.h"
#include "app.h"
#include "action.h"
#include "settings.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QPushButton>
#include <QAction>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include "panes/panes.h"

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

void MainWin::initLayout() {
  // layout helpers
  typedef QHBoxLayout HBox;
  typedef QVBoxLayout VBox;

  auto ctrWgt = new QWidget();
  setCentralWidget(ctrWgt);

  // three columns
  auto *hl = new HBox(ctrWgt);
  auto *v1 = new VBox;
  auto *v2 = new VBox;
  auto *v3 = new VBox;

  hl->addLayout(v1,0);
  hl->addLayout(v2,1);
  hl->addLayout(v3,0);

  // panes
  v1->addWidget(new Files());
  v1->addWidget(new Detector());
  v1->addWidget(new Background());
  v1->addWidget(new Reflections());

  auto *v2h = new HBox;
  v2->addLayout(v2h,1);

  v2h->addWidget(new Images());
  v2h->addWidget(new Image());

  v2h = new HBox;
  v2->addLayout(v2h,1);
  v2h->addWidget(new Diffractogram());

  v3->addWidget(new ImageInfo());
  v3->addWidget(new ReflectionInfo());
  v3->addWidget(new Normalization());

  auto *v3h = new HBox;
  v3->addLayout(v3h);

  v3h->addWidget(new QPushButton("Pole figure..."));
  v3h->addWidget(new QPushButton("Diagram..."));
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
      actViewMenubar    = toggle("Menubar",    true, Settings::KEY_VIEW_MENU) // TODO actions owned elsewhere
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
    separator(),
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

  connect(actQuit, &QAction::triggered, [&](){ close(); });

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
