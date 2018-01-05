// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/thehub.h
//! @brief     Defines class TheHub
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef THEHUB_H
#define THEHUB_H

#include "calc/lens.h"
#include "calc/calc_reflection_info.h"
#include "calc/calc_reflection.h"
#include "data/datafile.h"

class Suite;

class QAction;

class FilesModel;
class DatasetsModel;
class MetadataModel;
class ReflectionsModel;


// make connects shorter
#define slot(Type, method, parType) static_cast<void (Type::*)(parType)>(&Type::method)

namespace gui {

extern class TheHub* gHub;

enum class eFittingTab {
    NONE,
    BACKGROUND,
    REFLECTIONS,
};

class TheHub : public QObject {
private:
    Q_OBJECT

    TheHub& asHub();

public: // emit signals
    void tellSuiteSelected(QSharedPointer<Suite>);
    void tellSelectedReflection(shp_Reflection);
    void tellReflectionData(shp_Reflection);
    void tellReflectionValues(Range const&, qpair const&, fwhm_t, bool);

signals:
    void sigFilesChanged(); // the set of loaded files has changed
    void sigFilesSelected(); // the selection of loaded files has changed

    void sigSuitesChanged(); // the set of suite collected from selected
    // files has changed
    void sigSuiteSelected(QSharedPointer<Suite>);

    void sigCorrFile(QSharedPointer<Datafile const>);
    void sigCorrEnabled(bool);

    void sigReflectionsChanged();
    void sigReflectionSelected(shp_Reflection);
    void sigReflectionData(shp_Reflection);
    void sigReflectionValues(Range const&, qpair const&, fwhm_t, bool);

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
    void sessionFromFile(rcstr const&) THROWS;
    void addGivenFile(rcstr filePath) THROWS;
    void addGivenFiles(QStringList const& filePaths) THROWS;
    void collectDatasetsFromFiles(uint_vec, pint);
    void collectDatasetsFromFiles(uint_vec);
    void combineDatasetsBy(pint);
    void setCorrFile(rcstr filePath) THROWS;
    void tryEnableCorrection(bool);
    void setImageCut(bool isTopOrLeft, bool linked, ImageCut const&);
    void setGeometry(preal detectorDistance, preal pixSize, IJ const& midPixOffset);
    void setGammaRange(Range const&);

    void setBgRanges(Ranges const&);
    void addBgRange(Range const&);
    void remBgRange(Range const&);
    void setBgPolyDegree(uint);

    void setIntenScaleAvg(bool, preal);
    void setNorm(eNorm);
    void setFittingTab(eFittingTab);

    void setPeakFunction(QString const&);
    void addReflection(QString const&);
    void remReflection(uint);

    // const methods:
    bool isFixedIntenImageScale() const { return isFixedIntenImageScale_; }
    bool isFixedIntenDgramScale() const { return isFixedIntenDgramScale_; }
    bool isCombinedDgram() const { return isCombinedDgram_; }

    void saveSession(QFileInfo const&) const;
    QByteArray saveSession() const;

    pint suiteGroupedBy() const { return suiteGroupedBy_; }

    eFittingTab fittingTab() const { return fittingTab_; }

    QSharedPointer<Suite> selectedSuite() const { return selectedSuite_; }

private:
    friend class TheHubSignallingBase;
    bool isFixedIntenImageScale_;
    bool isFixedIntenDgramScale_;
    bool isCombinedDgram_;
    uint_vec collectFromFiles_;
    pint suiteGroupedBy_ = pint(1);
    eFittingTab fittingTab_ = eFittingTab::NONE;
    QSharedPointer<Suite> selectedSuite_;
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

} // namespace gui

#endif // THEHUB_H
