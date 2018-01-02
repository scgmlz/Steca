// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/session.h
//! @brief     Defines class Session
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef SESSION_H
#define SESSION_H

#include "calc/calc_lens.h"
#include "calc/calc_reflection.h"
#include "calc/calc_reflection_info.h"
#include "data/angle_map.h"
#include "data/datafile.h"
#include "typ/async.h"
#include "typ/cache.h"
#include "typ/singleton.h"

extern class Session* gSession;

class Session final : public ISingleton<Session> {
public:
    Session();

private:
    vec<shp_Datafile> files_; //!< data files
    shp_Datafile corrFile_; //!< correction file
    shp_Image corrImage_;
    bool corrEnabled_;
    uint_vec collectedFromFiles_; // from these files
    Experiment collectedDatasets_; // datasets collected ...
    QStringList collectedDatasetsTags_;
    bool intenScaledAvg_; // if not, summed
    preal intenScale_;
    size2d imageSize_; //!< All images must have this same size
    ImageTransform imageTransform_;
    ImageCut imageCut_;
    Geometry geometry_;
    Range gammaRange_;
    uint bgPolyDegree_;
    Ranges bgRanges_;
    calc::Reflections reflections_;
    eNorm norm_;

    mutable Image intensCorr_;
    mutable bool corrHasNaNs_;
    mutable cache_lazy<AngleMap::Key, AngleMap> angleMapCache_;

    void updateImageSize(); //!< Clears image size if session has no files
    void setImageSize(size2d const&) THROWS; //!< Ensures same size for all images

    void calcIntensCorr() const;
    Curve curveMinusBg(calc::DatasetLens const&, Range const&) const;
    calc::ReflectionInfo makeReflectionInfo(
        calc::DatasetLens const&, calc::Reflection const&, Range const&) const;

public:
    // Modifying methods:
    void clear();
    void addGivenFile(shp_Datafile) THROWS;
    void removeFile(uint i);
    void setCorrFile(shp_Datafile) THROWS; // Load or remove a correction file.
    void remCorrFile();
    void collectDatasetsFromFiles(uint_vec, pint);

    void setImageTransformMirror(bool);
    void setImageTransformRotate(ImageTransform const&);
    void setImageCut(bool isTopOrLeft, bool linked, ImageCut const&);
    void setGeometry(preal detectorDistance, preal pixSize, IJ const& midPixOffset);
    void setGammaRange(Range const& r) { gammaRange_ = r; }
    void setBgRanges(Ranges const& rr) { bgRanges_ = rr; }
    bool addBgRange(Range const& r) { return bgRanges_.add(r); }
    bool remBgRange(Range const& r) { return bgRanges_.rem(r); }
    void setBgPolyDegree(uint degree) { bgPolyDegree_ = degree; }
    void setIntenScaleAvg(bool, preal);
    void addReflection(QString const&);
    void addReflection(const QJsonObject& obj);
    void remReflection(uint i) { reflections_.remove(i); }
    void setNorm(eNorm norm) { norm_ = norm; }

    // Const methods:
    uint numFiles() const { //!< number of data files (not counting the correction file)
        return files_.count(); }
    shp_Datafile file(uint i) const { return files_.at(i); }
    bool hasFile(rcstr fileName) const;
    bool hasCorrFile() const { return !corrFile_.isNull(); }
    shp_Datafile corrFile() const { return corrFile_; }
    shp_Image corrImage() const { return corrImage_; }
    Image const* intensCorr() const;
    void tryEnableCorr(bool on) { corrEnabled_ = on && hasCorrFile(); }
    bool isCorrEnabled() const { return corrEnabled_; }

    uint_vec const& collectedFromFiles() const { return collectedFromFiles_; }
    Experiment const& collectedDatasets() const { return collectedDatasets_; }
    QStringList const& collectedDatasetsTags() const { return collectedDatasetsTags_; }

    size2d imageSize() const;
    ImageTransform const& imageTransform() const { return imageTransform_; }
    ImageCut const& imageCut() const { return imageCut_; }

    Geometry const& geometry() const { return geometry_; }
    IJ midPix() const;

    Range const& gammaRange() const { return gammaRange_; }

    shp_AngleMap angleMap(Measurement const&) const;
    static shp_AngleMap angleMap(Session const& session, Measurement const& ds) {
        return session.angleMap(ds); }

    calc::shp_ImageLens imageLens(Image const&, Experiment const&, bool trans, bool cut) const;
    calc::shp_DatasetLens datasetLens(
        DataSequence const&, Experiment const&, eNorm, bool trans, bool cut) const;

    calc::ReflectionInfos makeReflectionInfos(
        Experiment const&, calc::Reflection const&, uint gmaSlices, Range const&, Progress*) const;

    Ranges const& bgRanges() const { return bgRanges_; }
    uint bgPolyDegree() const { return bgPolyDegree_; }
    bool intenScaledAvg() const { return intenScaledAvg_; }
    preal intenScale() const { return intenScale_; }
    calc::Reflections const& reflections() const { return reflections_; }

    eNorm norm() const { return norm_; }

    qreal calcAvgBackground(DataSequence const&) const;
    qreal calcAvgBackground(Experiment const&) const;
};

#endif // SESSION_H
