// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/thehub.h
//! @brief     Defines classes Settings, ReadFile, WriteFile, TheHub
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

#include <QSettings>

class QSpinBox;
class QDoubleSpinBox;

namespace gui {

class Settings : public QSettings {
    CLASS(Settings) SUPER(QSettings);
public:
    Settings(rcstr group = EMPTY_STR);
    ~Settings();

    QVariant readVariant(rcstr key, QVariant const& def);
    void saveVariant(rcstr key, QVariant const& val) { setValue(key, val); }

    void read(rcstr key, QAction*, bool def = false);
    void save(rcstr key, QAction*);

    void read(rcstr key, QSpinBox*, int def = 0);
    void save(rcstr key, QSpinBox*);

    void read(rcstr key, QDoubleSpinBox*, qreal def = 0);
    void save(rcstr key, QDoubleSpinBox*);

    bool readBool(rcstr key, bool def = false) { return readVariant(key, def).toBool(); }
    void saveBool(rcstr key, bool val) { saveVariant(key, val); }

    int readInt(rcstr key, int def = 0);
    void saveInt(rcstr key, int val) { saveVariant(key, val); }

    qreal readReal(rcstr key, qreal def = 0);
    void saveReal(rcstr key, qreal val) { saveVariant(key, val); }

    str readStr(rcstr key, rcstr def = EMPTY_STR) { return readVariant(key, def).toString(); }
    void saveStr(rcstr key, rcstr val) { saveVariant(key, val); }
};


class ReadFile : public QFile {
    CLASS(ReadFile) SUPER(QFile);
public:
    ReadFile(rcstr path) THROWS;
};


class WriteFile : public QFile {
    CLASS(WriteFile) SUPER(QFile);
public:
    WriteFile(rcstr path) THROWS;
};


class TheHub : public TheHubSignallingBase {
    CLASS(TheHub) SUPER(TheHubSignallingBase);
    friend class TheHubSignallingBase;
public:
    TheHub();

    Actions actions;

private:
    void configActions();

    scoped<core::Session*> session_;

    bool isFixedIntenImageScale_;
    bool isFixedIntenDgramScale_;
    bool isCombinedDgram_;

public:
    bool isFixedIntenImageScale() const { return isFixedIntenImageScale_; }
    bool isFixedIntenDgramScale() const { return isFixedIntenDgramScale_; }
    bool isCombinedDgram() const { return isCombinedDgram_; }

    models::FilesModel filesModel;
    models::DatasetsModel datasetsModel;
    models::MetadataModel metadataModel;
    models::ReflectionsModel reflectionsModel;

    // files
    uint numFiles() const { return session_->numFiles(); }
    str fileName(uint index) const { return getFile(index)->fileName(); }
    str filePath(uint index) const { return getFile(index)->fileInfo().absoluteFilePath(); }
    data::shp_File getFile(uint index) const { return session_->file(index); }
    void remFile(uint);

    bool hasCorrFile() const;
    typ::shp_Image corrImage() const { return session_->corrImage(); }

    calc::shp_ImageLens plainImageLens(typ::Image::rc) const;
    calc::shp_DatasetLens datasetLens(data::Dataset::rc) const;

    typ::Curve avgCurve(data::Datasets::rc dss) const { return dss.avgCurve(*session_); }

    calc::ReflectionInfos makeReflectionInfos(
        calc::Reflection::rc, uint gmaSlices, gma_rge::rc, Progress*);

    void saveSession(QFileInfo const&) const;
    QByteArray saveSession() const;

    void clearSession();
    void loadSession(QFileInfo const&) THROWS;
    void loadSession(QByteArray const&) THROWS;

    void addFile(rcstr filePath) THROWS;
    void addFiles(str_lst::rc filePaths) THROWS;

private:
    uint_vec collectFromFiles_;
    pint datasetsGroupedBy_ = pint(1);

public:
    void collectDatasetsFromFiles(uint_vec, pint);
    void collectDatasetsFromFiles(uint_vec);
    void combineDatasetsBy(pint);

    uint_vec::rc collectedFromFiles() const { return session_->collectedFromFiles(); }

    pint datasetsGroupedBy() const { return datasetsGroupedBy_; }

    uint numCollectedDatasets() const { return collectedDatasets().count(); }

    data::Datasets::rc collectedDatasets() const { return session_->collectedDatasets(); }

    str_lst::rc collectedDatasetsTags() const { return session_->collectedDatasetsTags(); }

    typ::size2d imageSize() const { return session_->imageSize(); }

    typ::shp_AngleMap angleMap(data::OneDataset::rc dataset) const {
        return session_->angleMap(dataset);
    }

    gma_rge collectedDatasetsRgeGma() const;

    void setCorrFile(rcstr filePath) THROWS;
    void tryEnableCorrection(bool);

    typ::ImageCut::rc imageCut() const;
    void setImageCut(bool topLeft, bool linked, typ::ImageCut::rc);

    typ::Geometry::rc geometry() const;
    void setGeometry(preal detectorDistance, preal pixSize, typ::IJ::rc midPixOffset);

    typ::Range::rc gammaRange() const { return session_->gammaRange(); }
    void setGammaRange(typ::Range::rc);

    void setBgRanges(typ::Ranges::rc);
    void addBgRange(typ::Range::rc);
    void remBgRange(typ::Range::rc);

    static uint constexpr MAX_POLYNOM_DEGREE = 4;
    void setBgPolyDegree(uint);

    void setIntenScaleAvg(bool, preal);

    void setReflType(fit::ePeakType);

    void addReflection(fit::ePeakType);
    void remReflection(uint);

    eFittingTab fittingTab() const { return fittingTab_; }
    void setFittingTab(eFittingTab);

private:
    eFittingTab fittingTab_ = eFittingTab::NONE;

    data::shp_Dataset selectedDataset_;
    calc::shp_Reflection selectedReflection_;

public:
    data::shp_Dataset selectedDataset() const { return selectedDataset_; }

private:
    void setImageRotate(typ::ImageTransform);
    void setImageMirror(bool);

public:
    void setNorm(eNorm);

    typ::Ranges::rc bgRanges() const { return session_->bgRanges(); }
    uint bgPolyDegree() const { return session_->bgPolyDegree(); }

    bool intenScaledAvg() const { return session_->intenScaledAvg(); }
    preal intenScale() const { return session_->intenScale(); }

    calc::Reflections::rc reflections() const { return session_->reflections(); }
};

} // namespace gui

#endif
