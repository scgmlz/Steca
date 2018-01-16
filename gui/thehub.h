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
#include "core/calc/reflection.h"
#include "core/data/datafile.h"
#include "core/data/cluster.h"
#include "gui/cfg/settings.h"

// make connects shorter
#define _SLOT_(Type, method, parType) static_cast<void (Type::*)(parType)>(&Type::method)

extern class TheHub* gHub; //!< global pointer to _the_ instance of TheHub

enum class eFittingTab {
    NONE,
    BACKGROUND,
    REFLECTIONS,
};

//! Companion of MainWin, holds signals and methods for interaction between views and data.

//! One instance of this class coexists with the main window. It is accessible from everywhere
//! through the global pointer gHub.

//! The original idea was that _all_ signalling passes through this class. In the big
//! refactoring after v2.0.5, this idea has been given up.

//! We should consider merging TheHub into MainWin.

class TheHub : public QObject, public ISingleton<TheHub> {
private:
    Q_OBJECT

public: // emit signals
    void tellClusterSelected(shp_Cluster);
    void tellSelectedReflection(shp_Reflection);
    void tellReflectionData(shp_Reflection);
    void tellReflectionValues(const Range&, qpair const&, fwhm_t, bool);

signals:
    void sigFilesChanged(); //!< loaded file set has changed
    void sigFilesSelected(); //!< active file selection has changed

    void sigClustersChanged(); //!< the set of cluster collected from selected
    void sigClusterSelected(shp_Cluster);

    void sigCorrFile(const Datafile*);
    void sigCorrEnabled(bool);

    void sigReflectionsChanged();
    void sigReflectionSelected(shp_Reflection);
    void sigReflectionData(shp_Reflection);
    void sigReflectionValues(const Range&, qpair const&, fwhm_t, bool);

    void sigDisplayChanged();
    void sigGeometryChanged();

    void sigGammaRange();

    void sigBgChanged(); // ranges and poly: refit
    void sigNormChanged();

    void sigFittingTab(eFittingTab);

    void sigMetatagsChosen(vec<bool>); //!< Selection of metadata has changed

public:
    TheHub();
    ~TheHub();

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
        *toggle_enableCorr,
        *trigger_removeCorr,
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
        *trigger_clearReflections,
        *trigger_addReflection,
        *trigger_removeReflection,
        *trigger_outputPolefigures,
        *trigger_outputDiagrams,
        *trigger_outputDiffractograms;

    // modifying methods:
    void removeFile(int);
    void sessionFromFile(rcstr&) THROWS;
    void addGivenFiles(const QStringList& filePaths) THROWS;
    void collectDatasetsFromSelection(const vec<int>);
    void combineMeasurementsBy(const int);
    void setCorrFile(rcstr filePath) THROWS;
    void tryEnableCorrection(bool);
    void setImageCut(bool isTopOrLeft, bool linked, ImageCut const&);
    void setGeometry(qreal detectorDistance, qreal pixSize, IJ const& midPixOffset);
    void setGammaRange(const Range&);

    void setBgRanges(const Ranges&);
    void addBgRange(const Range&);
    void removeBgRange(const Range&);
    void setBgPolyDegree(int);

    void setIntenScaleAvg(bool, qreal);
    void setNorm(eNorm);
    void setFittingTab(eFittingTab);

    void setPeakFunction(const QString&);
    void addReflection(const QString&);
    void removeReflection(int);

    // const methods:
    bool isFixedIntenImageScale() const { return isFixedIntenImageScale_; }
    bool isFixedIntenDgramScale() const { return isFixedIntenDgramScale_; }
    bool isCombinedDgram() const { return isCombinedDgram_; }

    void saveSession(QFileInfo const&) const;
    QByteArray saveSession() const;

    int clusterGroupedBy() const { return clusterGroupedBy_; }

    eFittingTab fittingTab() const { return fittingTab_; }

    shp_Cluster selectedCluster() const { return selectedCluster_; }

private:
    friend class TheHubSignallingBase;
    bool isFixedIntenImageScale_;
    bool isFixedIntenDgramScale_;
    bool isCombinedDgram_;
    vec<int> filesSelection_;
    int clusterGroupedBy_ = 1;
    eFittingTab fittingTab_ = eFittingTab::NONE;
    shp_Cluster selectedCluster_;
    shp_Reflection selectedReflection_;
    Settings settings_;

    void collectDatasetsFromSelectionBy(const vec<int>, const int);
    void collectDatasetsExec();
    void setImageRotate(ImageTransform);
    void setImageMirror(bool);
    void configActions();
    void sessionFromJson(QByteArray const&) THROWS;

public: // TODO relagate this to TabSave or similar
    str saveDir; //!< setting: default directory for data export
    str saveFmt; //!< setting: default format for data export
};

#endif // THEHUB_H
