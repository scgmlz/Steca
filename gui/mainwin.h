// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/mainwin.h
//! @brief     Defines class MainWin
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef MAINWIN_H
#define MAINWIN_H

#include "typ/singleton.h"
#include "typ/str.h"
#include <QMainWindow>
#include <QNetworkAccessManager>

namespace gui {

extern class MainWin* gMainWin;

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

} // namespace gui

#endif // MAINWIN_H
