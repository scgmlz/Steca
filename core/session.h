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

#ifndef CORE_SESSION_H
#define CORE_SESSION_H

#include "calc/calc_lens.h"
#include "calc/calc_reflection.h"
#include "calc/calc_reflection_info.h"
#include "data/data_file.h"
#include "typ/typ_async.h"
#include "typ/typ_cache.h"

namespace core {

class Session final {
    CLASS(Session)
public:
    Session();

    void clear();

private:
    typ::vec<data::shp_File> files_; //!< data files

public:
    uint numFiles() const { //!< number of data files (not counting the correction file)
        return files_.count(); }
    data::shp_File file(uint i) const { return files_.at(i); }

    bool hasFile(rcstr fileName);
    void addFile(data::shp_File) THROWS;
    void remFile(uint i);

private:
    data::shp_File corrFile_; //!< correction file
    typ::shp_Image corrImage_;
    bool corrEnabled_;

    mutable typ::Image intensCorr_;
    mutable bool corrHasNaNs_;

    void calcIntensCorr() const;

    // datasets
    uint_vec collectedFromFiles_; // from these files
    data::Datasets collectedDatasets_; // datasets collected ...
    str_lst collectedDatasetsTags_;

    // scaling
    bool intenScaledAvg_; // if not, summed
    preal intenScale_;

public:
    bool hasCorrFile() const { return !corrFile_.isNull(); }

    data::shp_File corrFile() const { return corrFile_; }

    typ::shp_Image corrImage() const { return corrImage_; }

    typ::Image const* intensCorr() const;

    void setCorrFile(data::shp_File) THROWS; // Load or remove a correction file.
    void remCorrFile();

    void tryEnableCorr(bool on) { corrEnabled_ = on && hasCorrFile(); }

    bool isCorrEnabled() const { return corrEnabled_; }

    void collectDatasetsFromFiles(uint_vec, pint);

    uint_vec::rc collectedFromFiles() const { return collectedFromFiles_; }

    data::Datasets::rc collectedDatasets() const { return collectedDatasets_; }

    str_lst::rc collectedDatasetsTags() const { return collectedDatasetsTags_; }

private:
    // All files must have images of the same size
    typ::size2d imageSize_;
    // Clears the image size if there are no files in the session.
    void updateImageSize();
    // Ensures that all images have the same size.
    void setImageSize(typ::size2d::rc) THROWS;

public:
    typ::size2d imageSize() const;

    // image - transform & cut etc.
private:
    typ::ImageTransform imageTransform_;
    typ::ImageCut imageCut_;

public:
    typ::ImageTransform::rc imageTransform() const { return imageTransform_; }
    typ::ImageCut::rc imageCut() const { return imageCut_; }

    void setImageTransformMirror(bool);
    void setImageTransformRotate(typ::ImageTransform::rc);

    void setImageCut(bool topLeftFirst, bool linked, typ::ImageCut::rc);

private:
    typ::Geometry geometry_;
    typ::Range gammaRange_;
    mutable typ::cache_lazy<typ::AngleMap::Key, typ::AngleMap> angleMapCache_;

public:
    typ::Geometry::rc geometry() const { return geometry_; }
    void setGeometry(preal detectorDistance, preal pixSize, typ::IJ::rc midPixOffset);
    typ::IJ midPix() const;

    typ::Range::rc gammaRange() const { return gammaRange_; }
    void setGammaRange(typ::Range::rc r) { gammaRange_ = r; }

    typ::shp_AngleMap angleMap(data::OneDataset::rc) const;
    static typ::shp_AngleMap angleMap(Session::rc session, data::OneDataset::rc ds) {
        return session.angleMap(ds); }

    // lenses
public:
    calc::shp_ImageLens imageLens(typ::Image::rc, data::Datasets::rc, bool trans, bool cut) const;
    calc::shp_DatasetLens
    datasetLens(data::Dataset::rc, data::Datasets::rc, eNorm, bool trans, bool cut) const;
    typ::Curve makeCurve(calc::DatasetLens::rc, gma_rge::rc) const;

    // reflections
    calc::ReflectionInfo
        makeReflectionInfo(calc::DatasetLens::rc, calc::Reflection::rc, gma_rge::rc) const;

    calc::ReflectionInfos makeReflectionInfos(
        data::Datasets::rc, calc::Reflection::rc, uint gmaSlices, gma_rge::rc, Progress*);

    // fitting
private:
    uint bgPolyDegree_;
    typ::Ranges bgRanges_;

    calc::Reflections reflections_;

public:
    typ::Ranges::rc bgRanges() const { return bgRanges_; }
    uint bgPolyDegree() const { return bgPolyDegree_; }
    bool intenScaledAvg() const { return intenScaledAvg_; }
    preal intenScale() const { return intenScale_; }
    calc::Reflections::rc reflections() const { return reflections_; }

    void setBgRanges(typ::Ranges::rc rr) { bgRanges_ = rr; }
    bool addBgRange(typ::Range::rc r) { return bgRanges_.add(r); }
    bool remBgRange(typ::Range::rc r) { return bgRanges_.rem(r); }

    void setBgPolyDegree(uint degree) { bgPolyDegree_ = degree; }
    void setIntenScaleAvg(bool, preal);

    void addReflection(calc::shp_Reflection);
    void remReflection(uint i) { reflections_.remove(i); }

    // normalisation
private:
    eNorm norm_;

public:
    eNorm norm() const { return norm_; }
    void setNorm(eNorm norm) { norm_ = norm; }

public:
    qreal calcAvgBackground(data::Dataset::rc) const;
    qreal calcAvgBackground(data::Datasets::rc) const;
};

} // namespace core

#endif
