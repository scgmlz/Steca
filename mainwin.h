#ifndef MAINWIN_H
#define MAINWIN_H

#include "defs.h"
#include <QMainWindow>

class QMenu; class QAction;

class MainWin: public QMainWindow {
  SUPER(MainWin,QMainWindow) Q_OBJECT
public:
  MainWin();
 ~MainWin();

private:
  void init();

  void initLayout();
  void initActionsAndMenus();
  void initStatus();

  QMenu *menuFile, *menuEdit, *menuView, *menuOpts, *menuHelp;

  QAction
    *actAddFiles, *actOpenCorrectionFile,
    *actOpenSession, *actSaveSession,

    *actExportDiffractogramCurrent,
    *actExportDiffractogramAllSeparateFiles,
    *actExportDiffractogramSingleFile,
    *actExportImagesWithMargins,
    *actExportImagesWithoutMargins,

    *actQuit,

    *actUndo, *actRedo,
    *actCut,  *actCopy, *actPaste,

    *actViewMenubar,
    *actViewStatusbar,
    *actFullscreen,

    *actPreferences,
    *actFitErrorParameters,

    *actPdfManual,
    *actAbout;

  void closeEvent(QCloseEvent*);
  bool onClose();

private:
  QByteArray initialState;

  void readSettings();
  void saveSettings();
};

#endif
