// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      mainwin.h
//! @brief     The main window
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef MAINWIN_H
#define MAINWIN_H

#include "core_defs.h"
#include "thehub.h"
#include <QMainWindow>

//------------------------------------------------------------------------------

class MainWin : public QMainWindow {
  SUPER(MainWin, QMainWindow)
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
  void about();

  void show();
  void close();

  void addFiles();
  void loadCorrFile();

  void loadSession();
  void saveSession();

  void outputPoleFigures();

private:
  // the hub
  TheHub theHub;

private:
  void closeEvent(QCloseEvent*);

  void onShow();
  void onClose();

private:
  QMenu *menuFile, *menuEdit, *menuView, *menuDatasets, *menuReflect,
        *menuOutput, *menuHelp;

  QDockWidget *dockFiles, *dockDatasets, *dockDatasetInfo;

private:
  QByteArray initialState;

  void readSettings();
  void saveSettings();

  void checkActions();

  void viewStatusbar(bool);
  void viewFullscreen(bool);
  void viewDockFiles(bool);
  void viewDockDatasets(bool);
  void viewDockDatasetInfo(bool);
  void viewReset();
};

//------------------------------------------------------------------------------
#endif // MAINWIN_H
