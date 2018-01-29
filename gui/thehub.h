// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/thehub.h
//! @brief     Defines the macro slot, the enum eFittingTab, the global gHub, and the class TheHub
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef THEHUB_H
#define THEHUB_H

#include "core/calc/lens.h"
#include "core/calc/peak.h"
#include "core/data/rawfile.h"
#include "core/data/cluster.h"
#include "gui/cfg/settings.h"

// make connects shorter
#define _SLOT_(Type, method, parType) static_cast<void (Type::*)(parType)>(&Type::method)

extern class TheHub* gHub; //!< global pointer to _the_ instance of TheHub

//! Indicates active setup tab, which determines which fit is to be shown in the diffractogram view.
enum class eFittingTab { NONE, BACKGROUND, REFLECTIONS, };

//! Companion of MainWin, holds signals and methods for interaction between views and data.

//! One instance of this class coexists with the main window. It is accessible from everywhere
//! through the global pointer gHub.

//! The original idea was that _all_ signalling passes through this class. In the big
//! refactoring after v2.0.5, this idea has been given up.

//! We should consider merging TheHub into MainWin.

class TheHub : public QObject, public ISingleton<TheHub> {
    Q_OBJECT

public:
    TheHub();
    ~TheHub();

signals: // TODO: rm unused

    void sigDisplayChanged();
    void sigFittingTab(eFittingTab);

public:
    // modifying methods:
    void sessionFromFile(rcstr&) THROWS;
    void loadCorrFile() THROWS;

    void setNorm(eNorm);
    void setFittingTab(eFittingTab);

    // const methods:
    bool isFixedIntenImageScale() const { return isFixedIntenImageScale_; }
    bool isFixedIntenDgramScale() const { return isFixedIntenDgramScale_; }
    bool isCombinedDgram() const { return isCombinedDgram_; }

    void saveSession(QFileInfo const&) const;
    QByteArray saveSession() const;

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

private:
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

    friend class TheHubSignallingBase;

public: // TODO relagate this to TabSave or similar
    str saveDir; //!< setting: default directory for data export
    str saveFmt; //!< setting: default format for data export
};

#endif // THEHUB_H
