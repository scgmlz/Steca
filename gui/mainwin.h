#ifndef MAINWIN_H
#define MAINWIN_H

#include "defs.h"
#include "session.h"
#include <QMainWindow>

class Split;

class MainWin: public QMainWindow {
  SUPER(MainWin,QMainWindow) Q_OBJECT
public:
  MainWin();
 ~MainWin();

private:
  void initActions();
  void initMenus();
  void initLayout();
  void initStatus();
  void connectActions();

public:
  void show();
  void close();

  void addFiles();
  void setCorrectionFile();

  void loadSession();
  void saveSession();

private:
  // where to look for files
  str  sessionDir, dataDir;

private:
  void closeEvent(QCloseEvent*);
  bool onClose();

public:
  Session session;

private:
  QMenu *menuFile, *menuEdit, *menuView, *menuOpts, *menuHelp;

public:
  QAction
    *actAddFiles, *actRemoveFile,
    *actSetCorrectionFile,
    *actOpenSession, *actSaveSession,

    *actExportDiffractogramCurrent,
    *actExportDiffractogramAllSeparateFiles,
    *actExportDiffractogramSingleFile,
    *actExportImagesWithMargins,
    *actExportImagesWithoutMargins,

    *actQuit,

    *actUndo, *actRedo,
    *actCut,  *actCopy, *actPaste,

    *actViewStatusbar,
#ifndef Q_OS_OSX // Mac has its own
    *actFullscreen,
#endif
    *actViewReset,

    *actPreferences,
    *actFitErrorParameters,

    *actPdfManual,
    *actAbout,

  // more actions, probably not in the menu
    *actReflectionPeak, *actReflectionReflect, *actReflectionWidth, *actReflectionAdd,
    *actImagesCombine, *actImagesLink, *actImagesEye, *actImagesGlobalNorm,
    *actImagesUpDown, *actImagesLeftRight, *actImagesTurnRight, *actImagesTurnLeft,
    *actBackgroundBackground, *actBackgroundEye;

private:
  Split *splitFiles, *splitDatasets, *splitImage, *splitReflections, *splitDiffractogram;

private:
  QByteArray initialState;

  void readSettings();
  void saveSettings();

  void checkActions();

  void viewStatusbar(bool);
  void viewFullscreen(bool);
  void viewReset();
};

#endif
