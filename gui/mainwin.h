//  ***********************************************************************************************
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
//  ***********************************************************************************************

#ifndef MAINWIN_H
#define MAINWIN_H

#include "qcr/widgets/views.h"
#include <QDir>

extern class MainWin* gGui; //!< global pointer to _the_ main window

//! The main window. Its single instance is accessible through the global pointer gGui.

class MainWin : public QcrMainWindow {
    Q_OBJECT
public:
    MainWin();
    ~MainWin();

    const class Mainframe& mainframe() const { return *mainframe_; }
    const class SubframeSetup* setup() const { return frameSetup_; }

    class Triggers* triggers;
    class Toggles* toggles;
    class GuiState* state;
    class ImageTrafoActions* imageTrafoActions;

    // cross references for use in export; current state of editing
    const class BigtableModel* bigtableModel;
    bool editingBaseline {false};
    bool editingPeakfits {false};

private:
    void refresh();

    void readSettings();
    void saveSettings() const;
    void viewReset();

    void loadSession();
    void saveSession();
    void clearSession();

    void addFiles();
    void loadCorrFile();

    void runFits();
    void runInterpolation();

    class Mainframe* mainframe_;
    class SubframeDfgram* frameDfgram_;
    class SubframeSetup* frameSetup_;
    QcrDockWidget* dockFiles_;
    QcrDockWidget* dockClusters_;
    QcrDockWidget* dockMetadata_;
    class Menus* menus_;

    QByteArray initialState_;
    QDir sessionDir_ {QDir::homePath()};
    QDir dataDir_ {QDir::homePath()};

    friend Triggers;
    friend Toggles;
};

#endif // MAINWIN_H
