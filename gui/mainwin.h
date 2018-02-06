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
#include "core/calc/peak.h"
#include "core/data/rawfile.h"
#include "core/data/cluster.h"
#include "core/typ/singleton.h"
#include "core/typ/str.h"
#include "gui/cfg/settings.h"
#include <QMainWindow>
#include <QNetworkAccessManager>

extern class MainWin* gGui; //!< global pointer to _the_ main window

// make connects shorter
#define _SLOT_(Type, method, parType) static_cast<void (Type::*)(parType)>(&Type::method)

//! Indicates active setup tab, which determines which fit is to be shown in the diffractogram view.
enum class eFittingTab { NONE, BACKGROUND, REFLECTIONS, };


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
    void setFittingTab(eFittingTab);

    // const methods:
    bool isFixedIntenImageScale() const { return isFixedIntenImageScale_; }
    bool isFixedIntenDgramScale() const { return isFixedIntenDgramScale_; }
    bool isCombinedDgram() const { return isCombinedDgram_; }

    eFittingTab fittingTab() const { return fittingTab_; }

    static int constexpr MAX_POLYNOM_DEGREE = 4;

    QAction *trigger_about,
        *trigger_online,
        *trigger_checkUpdate,
        *trigger_quit,
        *toggle_viewStatusbar,
        *toggle_viewFiles,
        *toggle_viewDatasets,
        *toggle_viewMetadata,
        *trigger_viewReset,
#ifndef Q_OS_OSX // Mac has its own
        *toggle_fullScreen,
#endif
        *trigger_loadSession,
        *trigger_saveSession,
        *trigger_clearSession,
        *trigger_addFiles,
        *trigger_removeFile,
        *trigger_corrFile,
        *toggle_enableCorr,
        *trigger_rotateImage,
        *toggle_mirrorImage,
        *toggle_linkCuts,
        *toggle_showOverlay,
        *toggle_stepScale,
        *toggle_showBins,
        *toggle_fixedIntenImage,
        *toggle_fixedIntenDgram,
        *toggle_combinedDgram,
        *toggle_selRegions,
        *toggle_showBackground,
        *trigger_clearBackground,
        *trigger_clearPeaks,
        *trigger_addPeak,
        *trigger_removePeak,
        *trigger_outputPolefigures,
        *trigger_outputDiagrams,
        *trigger_outputDiffractograms;

    // TODO relagate this to TabSave or similar
    QString saveDir; //!< setting: default directory for data export
    QString saveFmt; //!< setting: default format for data export

signals:
    void sigDisplayChanged();
    void sigFittingTab(eFittingTab);

private:
    QDockWidget *dockFiles_, *dockMeasurements_, *dockDatasetInfo_;
    QByteArray initialState_;
    QNetworkAccessManager netMan_;

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

    bool isFixedIntenImageScale_;
    bool isFixedIntenDgramScale_;
    bool isCombinedDgram_;
    eFittingTab fittingTab_ = eFittingTab::NONE;
    Settings settings_;
};

#endif // MAINWIN_H
