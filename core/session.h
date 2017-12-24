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
#include "data/datafile.h"
#include "typ/async.h"
#include "typ/cache.h"
#include "typ/exception.h"
#include "typ/range.h"
#include "typ/str.h"
#include "typ/vec.h"

namespace core {

class Session final {
    public:
    Session();

private:
    typ::vec<data::shp_File> files_; //!< data files
    data::shp_File corrFile_; //!< correction file
    typ::shp_Image corrImage_;
    bool corrEnabled_;
    uint_vec collectedFromFiles_; // from these files
    data::Datasets collectedDatasets_; // datasets collected ...
    str_lst collectedDatasetsTags_;
    bool intenScaledAvg_; // if not, summed
    preal intenScale_;
    typ::size2d imageSize_; //!< All images must have this same size
    typ::ImageTransform imageTransform_;
    typ::ImageCut imageCut_;
    typ::Geometry geometry_;
    typ::Range gammaRange_;
    uint bgPolyDegree_;
    typ::Ranges bgRanges_;
    calc::Reflections reflections_;
    eNorm norm_;

    mutable typ::Image intensCorr_;
    mutable bool corrHasNaNs_;
    mutable typ::cache_lazy<typ::AngleMap::Key, typ::AngleMap> angleMapCache_;

    void updateImageSize(); //!< Clears image size if session has no files
    void setImageSize(typ::size2d const&) THROWS; //!< Ensures same size for all images

    void calcIntensCorr() const;
    typ::Curve curveMinusBg(calc::DatasetLens const&, typ::Range const&) const;
    calc::ReflectionInfo makeReflectionInfo(
        calc::DatasetLens const&, calc::Reflection const&, typ::Range const&) const;

public:
    // Modifying methods:
    void clear();
    void addGivenFile(data::shp_File) THROWS;
    void remFile(uint i);
    void setCorrFile(data::shp_File) THROWS; // Load or remove a correction file.
    void remCorrFile();
    void collectDatasetsFromFiles(uint_vec, pint);

    void setImageTransformMirror(bool);
    void setImageTransformRotate(typ::ImageTransform const&);
    void setImageCut(bool isTopOrLeft, bool linked, typ::ImageCut const&);
    void setGeometry(preal detectorDistance, preal pixSize, typ::IJ const& midPixOffset);
    void setGammaRange(typ::Range const& r) { gammaRange_ = r; }
    void setBgRanges(typ::Ranges const& rr) { bgRanges_ = rr; }
    bool addBgRange(typ::Range const& r) { return bgRanges_.add(r); }
    bool remBgRange(typ::Range const& r) { return bgRanges_.rem(r); }
    void setBgPolyDegree(uint degree) { bgPolyDegree_ = degree; }
    void setIntenScaleAvg(bool, preal);
    void addReflection(fit::ePeakType type);
    void addReflection(const QJsonObject& obj);
    void remReflection(uint i) { reflections_.remove(i); }
    void setNorm(eNorm norm) { norm_ = norm; }

    // Const methods:
    uint numFiles() const { //!< number of data files (not counting the correction file)
        return files_.count(); }
    data::shp_File file(uint i) const { return files_.at(i); }
    bool hasFile(rcstr fileName) const;
    bool hasCorrFile() const { return !corrFile_.isNull(); }
    data::shp_File corrFile() const { return corrFile_; }
    typ::shp_Image corrImage() const { return corrImage_; }
    typ::Image const* intensCorr() const;
    void tryEnableCorr(bool on) { corrEnabled_ = on && hasCorrFile(); }
    bool isCorrEnabled() const { return corrEnabled_; }

    uint_vec const& collectedFromFiles() const { return collectedFromFiles_; }
    data::Datasets const& collectedDatasets() const { return collectedDatasets_; }
    str_lst const& collectedDatasetsTags() const { return collectedDatasetsTags_; }

    typ::size2d imageSize() const;
    typ::ImageTransform const& imageTransform() const { return imageTransform_; }
    typ::ImageCut const& imageCut() const { return imageCut_; }

    typ::Geometry const& geometry() const { return geometry_; }
    typ::IJ midPix() const;

    typ::Range const& gammaRange() const { return gammaRange_; }

    typ::shp_AngleMap angleMap(data::OneDataset const&) const;
    static typ::shp_AngleMap angleMap(Session const& session, data::OneDataset const& ds) {
        return session.angleMap(ds); }

    calc::shp_ImageLens imageLens(typ::Image const&, data::Datasets const&, bool trans, bool cut) const;
    calc::shp_DatasetLens datasetLens(
        data::Dataset const&, data::Datasets const&, eNorm, bool trans, bool cut) const;

    calc::ReflectionInfos makeReflectionInfos(
        data::Datasets const&, calc::Reflection const&, uint gmaSlices, typ::Range const&, Progress*) const;

    typ::Ranges const& bgRanges() const { return bgRanges_; }
    uint bgPolyDegree() const { return bgPolyDegree_; }
    bool intenScaledAvg() const { return intenScaledAvg_; }
    preal intenScale() const { return intenScale_; }
    calc::Reflections const& reflections() const { return reflections_; }

    eNorm norm() const { return norm_; }

    qreal calcAvgBackground(data::Dataset const&) const;
    qreal calcAvgBackground(data::Datasets const&) const;
};

} // namespace core

#endif
