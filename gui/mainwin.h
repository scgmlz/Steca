/*******************************************************************************
 * STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#ifndef MAINWIN_H
#define MAINWIN_H

#include "thehub.h"
#include "def/defs.h"

#include <QMainWindow>
#include <QNetworkAccessManager>

namespace gui {
//------------------------------------------------------------------------------

class MainWin : public QMainWindow {
  CLS(MainWin) SUPER(QMainWindow)
public:
  MainWin();

private:
  void initMenus();
  void addActions(QMenu*, QList<QAction*>);
  void initLayout();
  void initStatusBar();
  void connectActions();

public:
  void about();
  void online();
  void checkUpdate();
  void checkUpdate(bool completeReport);

private:
  void messageDialog(rcstr title, rcstr text);

public:
  void show();
  void close();

  void addFiles();
  void enableCorr();

  void loadSession();
  void saveSession();
  void clearSession();

  void outputPoleFigures();
  void outputDiagrams();
  void outputDiffractograms();

private:
  gui::TheHub   hub_;
  Actions      &acts_;

private:
  void closeEvent(QCloseEvent*);

  void onShow();
  void onClose();

private:
  QMenu
      *menuFile_,   *menuView_,
      *menuImage_,  *menuDgram_,
      *menuOutput_, *menuHelp_;

  QDockWidget
      *dockFiles_, *dockDatasets_, *dockDatasetInfo_, *dockHelp_;

private:
  QByteArray initialState_;
  QNetworkAccessManager netMan_;

  void readSettings();
  void saveSettings();

  void checkActions();

  void viewStatusbar(bool);
  void viewFullScreen(bool);
  void viewFiles(bool);
  void viewDatasets(bool);
  void viewDatasetInfo(bool);
  void viewHelp(bool);
  void viewReset();
};

//------------------------------------------------------------------------------
}
#endif // MAINWIN_H
