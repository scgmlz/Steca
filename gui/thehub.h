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

#include "signalling.h" // defines base class TheHubSignallingBase
#include "calc/lens.h"
#include "calc/calc_reflection_info.h"

class QAction;

class FilesModel;
class DatasetsModel;
class MetadataModel;
class ReflectionsModel;

extern class gui::TheHub* gHub;

namespace gui {

class TheHub : public TheHubSignallingBase {
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
