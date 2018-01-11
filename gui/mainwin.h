// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/mainwin.h
//! @brief     Defines class MainWin
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef MAINWIN_H
#define MAINWIN_H

#include "core/typ/singleton.h"
#include "core/typ/str.h"
#include <QMainWindow>
#include <QNetworkAccessManager>

extern class MainWin* gMainWin; //!< global pointer to _the_ main window

//! The main window.

//! This is a singleton class that specializes QMainWindow.
//! The one instance of this class is accessible from everywhere through
//! the global pointer gMainWin.

//! The main window coexists with an instance of TheHub, which is responsible
//! for most of the dynamic functionality. The division of tasks between MainWin
//! and TheHub is somewhat arbitrary, and we should consider merging both classes.

class MainWin : public QMainWindow, public ISingleton<MainWin> {
public:
    MainWin();

    void online();
    void checkUpdate();

    void close();

    void addFiles();
    void enableCorr();

    void loadSession();
    void saveSession();
    void clearSession();

    void execCommand(str);

private:
    QDockWidget *dockFiles_, *dockDatasets_, *dockDatasetInfo_;
    QByteArray initialState_;
    QNetworkAccessManager netMan_;

    void initLayout();
    void connectActions();

    void messageDialog(rcstr title, rcstr text);
    void closeEvent(QCloseEvent*);

    void readSettings();
    void saveSettings();

    void viewStatusbar(bool);
    void viewFullScreen(bool);
    void viewFiles(bool);
    void viewDatasets(bool);
    void viewMetadata(bool);
    void viewReset();
};

#endif // MAINWIN_H
