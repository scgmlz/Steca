/** \file
 */

#ifndef MAINWIN_H
#define MAINWIN_H

#include "core_defs.h"
#include <QMainWindow>

class Session;
class BoxWidget;

//------------------------------------------------------------------------------

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
  void loadCorrectionFile();

  void loadSession();
  void saveSession();

private:
  // where to look for files
  str  sessionDir, dataDir;

private:
  void closeEvent(QCloseEvent*);
  void onClose();

private:
  Session *session;
  QMenu   *menuFile, *menuEdit, *menuView, *menuImage, *menuOpts, *menuHelp;

public:
  QAction
    *actAddFiles, *actRemoveFile,
    *actLoadCorrectionFile,
    *actLoadSession, *actSaveSession,

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

    *actAbout,

  // more actions, some not in the menu
    *actReflectionPeak, *actReflectionReflect, *actReflectionWidth, *actReflectionAdd,
    *actImagesLink, *actImageOverlay, *actImagesGlobalNorm, *actImagesEnableCorr,
    *actBackgroundBackground, *actBackgroundEye,
    *actHasBeamOffset;

private:
  QByteArray initialState;

  void readSettings();
  void saveSettings();

  void checkActions();

  void viewStatusbar(bool);
  void viewFullscreen(bool);
  void viewReset();
};

//------------------------------------------------------------------------------
#endif
