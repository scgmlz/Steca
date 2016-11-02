// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      mainwin.h
//! @brief     The main window
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle, Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

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
      *menuFile_, *menuView_,
      *menuImage_, *menuDgram_,
      *menuOutput_, *menuHelp_;

  QDockWidget
      *dockFiles_, *dockDatasets_, *dockDatasetInfo_;

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
  void viewReset();
};

//------------------------------------------------------------------------------
}
#endif // MAINWIN_H
