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
#include "core/typ/singleton.h"
#include "gui/cfg/settings.h"
#include <QMainWindow>
#include <QNetworkAccessManager>

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

    void online();
    void checkUpdate();

    void addFiles();

    void loadSession();
    void saveSession();
    void clearSession();

    // modifying methods:
    void sessionFromFile(const QString&) THROWS;
    void loadCorrFile() THROWS;

    void setNorm(eNorm);

    // const methods:
    bool isFixedIntenImageScale() const { return isFixedIntenImageScale_; }
    bool isFixedIntenDgramScale() const { return isFixedIntenDgramScale_; }
    bool isCombinedDgram() const { return isCombinedDgram_; }

    bool baselineEditable { false };
    bool peaksEditable { false };

    QAction* toggle_combinedDgram;
    QAction* toggle_enableCorr;
    QAction* toggle_fixedIntenDgram;
    QAction* toggle_fixedIntenImage;
#ifndef Q_OS_OSX // Mac has its own
    QAction* toggle_fullScreen;
#endif
    QAction* toggle_linkCuts;
    QAction* toggle_mirrorImage;
    QAction* toggle_showBackground;
    QAction* toggle_showBins;
    QAction* toggle_showOverlay;
    QAction* toggle_stepScale;
    QAction* toggle_viewDatasets;
    QAction* toggle_viewFiles;
    QAction* toggle_viewMetadata;
    QAction* toggle_viewStatusbar;
    QAction* trigger_about;
    QAction* trigger_addFiles;
    QAction* trigger_addPeak;
    QAction* trigger_checkUpdate;
    QAction* trigger_clearBackground;
    QAction* trigger_clearPeaks;
    QAction* trigger_clearSession;
    QAction* trigger_corrFile;
    QAction* trigger_loadSession;
    QAction* trigger_online;
    QAction* trigger_outputDiagrams;
    QAction* trigger_outputDiffractograms;
    QAction* trigger_outputPolefigures;
    QAction* trigger_quit;
    QAction* trigger_removeFile;
    QAction* trigger_removePeak;
    QAction* trigger_rotateImage;
    QAction* trigger_saveSession;
    QAction* trigger_viewReset;

    // TODO relagate this to TabSave or similar
    QString saveDir; //!< setting: default directory for data export
    QString saveFmt; //!< setting: default format for data export

private:
    QDockWidget *dockFiles_, *dockMeasurements_, *dockDatasetInfo_;
    QByteArray initialState_;
    QNetworkAccessManager netMan_;
    QMenu* menuDgram_;
    QMenu* menuImage_;
    QMenu* menuOutput_;

    void initMenu();
    void initLayout();
    void connectActions();

    void closeEvent(QCloseEvent*);

    void readSettings();
    void saveSettings();

    void saveSessionTo(QFileInfo const&);
    QByteArray serializeSession() const;

    void viewStatusbar(bool);
    void viewFullScreen(bool);
    void viewFiles(bool);
    void viewDatasets(bool);
    void viewMetadata(bool);
    void viewReset();

    void collectDatasetsFromSelectionBy(const vec<int>, const int);
    void setImageRotate(ImageTransform);
    void setImageMirror(bool);
    void configActions();
    void sessionFromJson(QByteArray const&) THROWS;
    void updateActionEnabling();

    bool isFixedIntenImageScale_;
    bool isFixedIntenDgramScale_;
    bool isCombinedDgram_;
    Settings settings_;
};

#endif // MAINWIN_H
