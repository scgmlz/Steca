#ifndef MAINWIN_H
#define MAINWIN_H

#include "defs.h"
#include <QMainWindow>

class QAction;
class QMenu;
class Panel;
class Docking;

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

private:
  str  dataFilesDir;  // where to begin to look for data files

private:
  void closeEvent(QCloseEvent*);
  bool onClose();

private:
  Docking *filesDock, *infoDock;
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

    *actViewFiles, *actViewInfo,
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
    *actImagesCombine, *actImagesLink, *actImagesEye,
    *actImagesUpDown, *actImagesLeftRight, *actImagesTurnRight, *actImagesTurnLeft,
    *actBackgroundBackground, *actBackgroundEye;

private:
  QByteArray initialState;

  void readSettings();
  void saveSettings();

  void checkActions();

  void viewFiles(bool);
  void viewInfo(bool);
  void viewStatusbar(bool);
  void viewFullscreen(bool);
  void viewReset();
};

#endif
