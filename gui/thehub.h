// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/thehub.h
//! @brief     Defines class TheHub
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef THEHUB_H
#define THEHUB_H

#include "actions.h"
#include "models.h"
#include "session.h"
#include "signalling.h"

namespace gui {

class TheHub : public TheHubSignallingBase {
public:
    TheHub();

    static uint constexpr MAX_POLYNOM_DEGREE = 4;

    QAction *actn_about, *actn_online, *actn_checkUpdate, *actn_quit, *actn_viewStatusbar, *actn_viewFiles, *actn_viewDatasets,
        *actn_viewDatasetInfo, *actn_viewReset,
#ifndef Q_OS_OSX // Mac has its own
        *actn_fullScreen,
#endif
        *actn_loadSession, *actn_saveSession, *actn_clearSession, *actn_addFiles, *actn_remFile, *actn_enableCorr, *actn_remCorr,
        *actn_rotateImage, *actn_mirrorImage, *actn_linkCuts, *actn_showOverlay, *actn_stepScale, *actn_showBins,
        *actn_fixedIntenImage, *actn_fixedIntenDgram, *actn_combinedDgram, *actn_selRegions, *actn_showBackground,
        *actn_clearBackground, *actn_clearReflections, *actn_addReflection, *actn_remReflection, *actn_outputPolefigures,
        *actn_outputDiagrams, *actn_outputDiffractograms;

    // modifying methods:
    void remFile(uint);
    calc::ReflectionInfos makeReflectionInfos(
        calc::Reflection const&, uint gmaSlices, typ::Range const&, Progress*);
    void clearSession();
    void sessionFromFile(QFileInfo const&) THROWS;
    void addGivenFile(rcstr filePath) THROWS;
    void addGivenFiles(QStringList const& filePaths) THROWS;
    void collectDatasetsFromFiles(uint_vec, pint);
    void collectDatasetsFromFiles(uint_vec);
    void combineDatasetsBy(pint);
    void setCorrFile(rcstr filePath) THROWS;
    void tryEnableCorrection(bool);
    void setImageCut(bool isTopOrLeft, bool linked, typ::ImageCut const&);
    void setGeometry(preal detectorDistance, preal pixSize, typ::IJ const& midPixOffset);
    void setGammaRange(typ::Range const&);

    void setBgRanges(typ::Ranges const&);
    void addBgRange(typ::Range const&);
    void remBgRange(typ::Range const&);
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

    uint numFiles() const { return session_->numFiles(); }
    str fileName(uint index) const { return getFile(index)->fileName(); }
    str filePath(uint index) const { return getFile(index)->fileInfo().absoluteFilePath(); }
    data::shp_File getFile(uint index) const { return session_->file(index); }

    bool hasCorrFile() const;
    typ::shp_Image corrImage() const { return session_->corrImage(); }

    calc::shp_ImageLens plainImageLens(typ::Image const&) const;
    calc::shp_DatasetLens datasetLens(data::Dataset const&) const;

    typ::Curve avgCurve(data::Datasets const& dss) const { return dss.avgCurve(*session_); }

    void saveSession(QFileInfo const&) const;
    QByteArray saveSession() const;

    uint_vec const& collectedFromFiles() const { return session_->collectedFromFiles(); }

    pint datasetsGroupedBy() const { return datasetsGroupedBy_; }

    uint numCollectedDatasets() const { return collectedDatasets().count(); }

    data::Datasets const& collectedDatasets() const { return session_->collectedDatasets(); }

    QStringList const& collectedDatasetsTags() const { return session_->collectedDatasetsTags(); }

    typ::size2d imageSize() const { return session_->imageSize(); }

    typ::shp_AngleMap angleMap(data::OneDataset const& dataset) const {
        return session_->angleMap(dataset);
    }

    typ::Range collectedDatasetsRgeGma() const;
    typ::ImageCut const& imageCut() const;
    typ::Geometry const& geometry() const;
    typ::Range const& gammaRange() const { return session_->gammaRange(); }

    eFittingTab fittingTab() const { return fittingTab_; }

    data::shp_Dataset selectedDataset() const { return selectedDataset_; }

    typ::Ranges const& bgRanges() const { return session_->bgRanges(); }
    uint bgPolyDegree() const { return session_->bgPolyDegree(); }

    bool intenScaledAvg() const { return session_->intenScaledAvg(); }
    preal intenScale() const { return session_->intenScale(); }

    calc::Reflections const& reflections() const { return session_->reflections(); }

private:
    friend class TheHubSignallingBase;
    scoped<Session*> session_;
    bool isFixedIntenImageScale_;
    bool isFixedIntenDgramScale_;
    bool isCombinedDgram_;
    uint_vec collectFromFiles_;
    pint datasetsGroupedBy_ = pint(1);
    eFittingTab fittingTab_ = eFittingTab::NONE;
    data::shp_Dataset selectedDataset_;
    calc::shp_Reflection selectedReflection_;

    void setImageRotate(typ::ImageTransform);
    void setImageMirror(bool);
    void configActions();
    void sessionFromJson(QByteArray const&) THROWS;
public:
    models::FilesModel filesModel;
    models::DatasetsModel datasetsModel;
    models::MetadataModel metadataModel;
    models::ReflectionsModel reflectionsModel;
};

} // namespace gui

#endif
