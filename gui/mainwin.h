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
#include "core/data/image_transform.h"
#include "core/typ/singleton.h"
#include "core/def/settings.h"
#include <QDir>
#include <QMainWindow>
#include <QSplitter>

extern class MainWin* gGui; //!< global pointer to _the_ main window

// make connects shorter
#define _SLOT_(Type, method, parType) static_cast<void (Type::*)(parType)>(&Type::method)

//! The main window. Its single instance is accessible through the global pointer gGui.

class MainWin : public QMainWindow {
    Q_OBJECT
public:
    MainWin();
    ~MainWin();

    class SubframeSetup* setup() const { return frameSetup_; }

    class Triggers* triggers;
    class Toggles* toggles;
    class ImageTrafoActions* imageTrafoActions;

private:
    void initLayout();
    void updateActionEnabling();

    void readSettings();
    void saveSettings() const;
    void viewReset();

    void loadSession();
    void saveSession();
    void clearSession();
    QByteArray serializeSession() const;
    void sessionFromJson(const QByteArray&) THROWS;

    void addFiles();
    void loadCorrFile() THROWS;

    QDockWidget* dockFiles_;
    QDockWidget* dockClusters_;
    QDockWidget* dockMetadata_;
    class SubframeDiffractogram* frameDiffractogram_;
    class SubframeImage* frameImage_;
    class SubframeSetup* frameSetup_;
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
