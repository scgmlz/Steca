// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/thehub.h
//! @brief     Defines class TheHub
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
#include "core/calc/reflection_info.h"
#include "core/calc/reflection.h"
#include "core/data/datafile.h"
#include "core/data/suite.h"

class QAction;

class FilesModel;
class DatasetsModel;
class MetadataModel;
class ReflectionsModel;

// make connects shorter
#define slot(Type, method, parType) static_cast<void (Type::*)(parType)>(&Type::method)

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

class TheHub : public QObject {
private:
    Q_OBJECT

    TheHub& asHub();

public: // emit signals
    void tellSuiteSelected(shp_Suite);
    void tellSelectedReflection(shp_Reflection);
    void tellReflectionData(shp_Reflection);
    void tellReflectionValues(const Range&, qpair const&, fwhm_t, bool);

signals:
    void sigFilesChanged(); // the set of loaded files has changed
    void sigFilesSelected(); // the selection of loaded files has changed

    void sigSuitesChanged(); // the set of suite collected from selected
    // files has changed
    void sigSuiteSelected(shp_Suite);

    void sigCorrFile(shp_Datafile);
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

public:
    TheHub();

    static uint constexpr MAX_POLYNOM_DEGREE = 4;

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
        *trigger_remCorr,
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
        *trigger_remReflection,
        *trigger_outputPolefigures,
        *trigger_outputDiagrams,
        *trigger_outputDiffractograms;

    // modifying methods:
    void removeFile(uint);
    void sessionFromFile(rcstr&) THROWS;
    void addGivenFile(rcstr filePath) THROWS;
    void addGivenFiles(const QStringList& filePaths) THROWS;
    void collectDatasetsFromFiles(uint_vec, pint);
    void collectDatasetsFromFiles(uint_vec);
    void combineDatasetsBy(pint);
    void setCorrFile(rcstr filePath) THROWS;
    void tryEnableCorrection(bool);
    void setImageCut(bool isTopOrLeft, bool linked, ImageCut const&);
    void setGeometry(preal detectorDistance, preal pixSize, IJ const& midPixOffset);
    void setGammaRange(const Range&);

    void setBgRanges(const Ranges&);
    void addBgRange(const Range&);
    void removeBgRange(const Range&);
    void setBgPolyDegree(uint);

    void setIntenScaleAvg(bool, preal);
    void setNorm(eNorm);
    void setFittingTab(eFittingTab);

    void setPeakFunction(const QString&);
    void addReflection(const QString&);
    void remReflection(uint);

    // const methods:
    bool isFixedIntenImageScale() const { return isFixedIntenImageScale_; }
    bool isFixedIntenDgramScale() const { return isFixedIntenDgramScale_; }
    bool isCombinedDgram() const { return isCombinedDgram_; }

    void saveSession(QFileInfo const&) const;
    QByteArray saveSession() const;

    pint suiteGroupedBy() const { return suiteGroupedBy_; }

    eFittingTab fittingTab() const { return fittingTab_; }

    shp_Suite selectedSuite() const { return selectedSuite_; }

private:
    friend class TheHubSignallingBase;
    bool isFixedIntenImageScale_;
    bool isFixedIntenDgramScale_;
    bool isCombinedDgram_;
    uint_vec collectFromFiles_;
    pint suiteGroupedBy_ = pint(1);
    eFittingTab fittingTab_ = eFittingTab::NONE;
    shp_Suite selectedSuite_;
    shp_Reflection selectedReflection_;

    void setImageRotate(ImageTransform);
    void setImageMirror(bool);
    void configActions();
    void sessionFromJson(QByteArray const&) THROWS;

public:
    FilesModel* filesModel;
    DatasetsModel* suiteModel;
    MetadataModel* metadataModel;
    ReflectionsModel* reflectionsModel;
};

#endif // THEHUB_H
