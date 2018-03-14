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
#include "gui/cfg/settings.h"
#include <QDir>
#include <QMainWindow>

extern class MainWin* gGui; //!< global pointer to _the_ main window

// make connects shorter
#define _SLOT_(Type, method, parType) static_cast<void (Type::*)(parType)>(&Type::method)

//! The main window.

//! This is a singleton class that specializes QMainWindow.
//! Its one instance is accessible from everywhere through the global pointer gGui.

class MainWin : public QMainWindow, public ISingleton<MainWin> {
    Q_OBJECT
public:
    MainWin();
    ~MainWin();

    void checkUpdate();

    void addFiles();

    void loadSession();
    void saveSession();
    void clearSession();

    // modifying methods:
    void sessionFromFile(const QString&) THROWS;
    void loadCorrFile() THROWS;

    void setImageRotate(ImageTransform);
    void viewReset();

    // const methods:
    bool isFixedIntenImageScale() const { return isFixedIntenImageScale_; }
    bool isFixedIntenDgramScale() const { return isFixedIntenDgramScale_; }
    bool isCombinedDgram() const { return isCombinedDgram_; }

    bool baselineEditable { false };
    bool peaksEditable { false };

    class Triggers* triggers;
    class Toggles* toggles;
    class Menus* menus;

    // TODO relegate this to TabSave or similar
    QString saveDir; //!< setting: default directory for data export
    QString saveFmt; //!< setting: default format for data export

private:
    QDockWidget *dockFiles_, *dockClusters_, *dockMetadata_;
    QByteArray initialState_;
    QDir sessionDir_ {QDir::homePath()};
    QDir dataDir_ {QDir::homePath()};
    const QString dataFormats_ {"Data files (*.dat *.mar*);;All files (*.*)"};

    void initMenu();
    void initLayout();

    void closeEvent(QCloseEvent*);

    void readSettings();
    void saveSettings();

    void saveSessionTo(const QFileInfo&);
    QByteArray serializeSession() const;

    void collectDatasetsFromSelectionBy(const vec<int>, const int);
    void setImageMirror(bool);
    void configActions();
    void sessionFromJson(const QByteArray&) THROWS;
    void updateActionEnabling();

    bool isFixedIntenImageScale_;
    bool isFixedIntenDgramScale_;
    bool isCombinedDgram_;
    Settings settings_;

    friend Triggers;
    friend Toggles;
    friend Menus;
};

#endif // MAINWIN_H
