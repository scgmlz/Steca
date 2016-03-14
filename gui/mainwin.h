/** \file
 */

#ifndef MAINWIN_H
#define MAINWIN_H

#include "core_defs.h"
#include "thehub.h"
#include <QMainWindow>

class BoxWidget;

//------------------------------------------------------------------------------

class MainWin: public QMainWindow {
  SUPER(MainWin,QMainWindow)
  Q_OBJECT
public:
  MainWin();
 ~MainWin();

private:
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
  // the hub
  TheHub theHub;

private:
  void closeEvent(QCloseEvent*);
  void onClose();

private:
  QMenu   *menuFile, *menuEdit, *menuView, *menuDatasets, *menuReflect, *menuHelp;

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
