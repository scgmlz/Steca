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

#include "core/calc/lens.h"
#include "core/data/rawfile.h"
#include "core/def/settings.h"
#include "gui/capture_and_replay/console.h"
#include <QDir>
#include <QMainWindow>
#include <QProgressBar>
#include <QSplitter>

extern class MainWin* gGui; //!< global pointer to _the_ main window

//! The main window. Its single instance is accessible through the global pointer gGui.

class MainWin : public QMainWindow {
    Q_OBJECT
public:
    MainWin();
    ~MainWin();

    const class Mainframe& mainframe() const { return *mainframe_; }
    const class SubframeSetup* setup() const { return frameSetup_; }

    class Triggers* triggers;
    class Toggles* toggles;
    class ImageTrafoActions* imageTrafoActions;

    QProgressBar progressBar {this};

private:
    Console console_;

    void initLayout();
    void updateActionEnabling();

    void readSettings();
    void saveSettings() const;
    void viewReset();

    void loadSession();
    void saveSession();
    void clearSession();

    void addFiles();
    void loadCorrFile() THROWS;

    void runFits();

    class Mainframe* mainframe_;
    class SubframeDiffractogram* frameDiffractogram_;
    class SubframeSetup* frameSetup_;
    QDockWidget* dockFiles_;
    QDockWidget* dockClusters_;
    QDockWidget* dockMetadata_;
    class Menus* menus_;

    QSplitter splMain_ {Qt::Vertical};
    QSplitter splTop_ {Qt::Horizontal};
    QByteArray initialState_;
    QDir sessionDir_ {QDir::homePath()};
    QDir dataDir_ {QDir::homePath()};

    friend Triggers;
    friend Toggles;
};

#endif // MAINWIN_H
