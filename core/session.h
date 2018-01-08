// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/session.h
//! @brief     Defines class Session
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef SESSION_H
#define SESSION_H

#include "core/calc/lens.h"
#include "core/calc/calc_reflection.h"
#include "core/calc/calc_reflection_info.h"
#include "core/data/angle_map.h"
#include "core/data/datafile.h"
#include "core/data/experiment.h"
#include "core/data/image.h"
#include "core/typ/async.h"
#include "core/typ/cache.h"
#include "core/typ/singleton.h"

extern class Session* gSession;

//! Companion of MainWin and TheHub, holds data and data-related settings.

//! One instance of this class coexists with the main window. It is accessible from everywhere
//! through the global pointer gSession.

//! The original idea was that core and GUI only communicate via function calls between
//! Session and TheHub. In the big refactoring after v2.0.5, this has been given up.

class Session final : public ISingleton<Session> {
public:
    Session();

private:
    vec<QSharedPointer<Datafile const>> files_; //!< data files
    QSharedPointer<Datafile const> corrFile_; //!< correction file
    QSharedPointer<Image> corrImage_;
    bool corrEnabled_;
    uint_vec collectedFromFiles_; // from these files
    Experiment experiment_; // suite collected ...
    QStringList experimentTags_;
    bool intenScaledAvg_; // if not, summed
    preal intenScale_;
    size2d imageSize_; //!< All images must have this same size
    ImageTransform imageTransform_;
    ImageCut imageCut_;
    Geometry geometry_;
    Range gammaRange_;
    uint bgPolyDegree_;
    Ranges bgRanges_;
    Reflections reflections_;
    eNorm norm_;

    mutable Image intensCorr_;
    mutable bool corrHasNaNs_;
    mutable cache_lazy<ImageKey, AngleMap> angleMapCache_;

    void updateImageSize(); //!< Clears image size if session has no files
    void setImageSize(size2d const&) THROWS; //!< Ensures same size for all images

    void calcIntensCorr() const;
    Curve curveMinusBg(SequenceLens const&, Range const&) const;
    ReflectionInfo makeReflectionInfo(
        SequenceLens const&, Reflection const&, Range const&) const;

public:
    // Modifying methods:
    void clear();
    void addGivenFile(QSharedPointer<Datafile const>) THROWS;
    void removeFile(uint i);
    void setCorrFile(QSharedPointer<Datafile const>) THROWS; // Load or remove a correction file.
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
    QSharedPointer<Datafile const> file(uint i) const { return files_.at(i); }
    bool hasFile(rcstr fileName) const;
    bool hasCorrFile() const { return !corrFile_.isNull(); }
    QSharedPointer<Datafile const> corrFile() const { return corrFile_; }
    QSharedPointer<Image> corrImage() const { return corrImage_; }
    Image const* intensCorr() const;
    void tryEnableCorr(bool on) { corrEnabled_ = on && hasCorrFile(); }
    bool isCorrEnabled() const { return corrEnabled_; }

    uint_vec const& collectedFromFiles() const { return collectedFromFiles_; }
    Experiment const& experiment() const { return experiment_; }
    QStringList const& experimentTags() const { return experimentTags_; }

    size2d imageSize() const;
    ImageTransform const& imageTransform() const { return imageTransform_; }
    ImageCut const& imageCut() const { return imageCut_; }

    Geometry const& geometry() const { return geometry_; }
    IJ midPix() const;

    Range const& gammaRange() const { return gammaRange_; }

    shp_AngleMap angleMap(Measurement const&) const;
    static shp_AngleMap angleMap(Session const& session, Measurement const& ds) {
        return session.angleMap(ds); }

    shp_ImageLens imageLens(Image const&, Experiment const&, bool trans, bool cut) const;
    QSharedPointer<SequenceLens> dataseqLens(Suite const&, eNorm, bool trans, bool cut) const;
    QSharedPointer<SequenceLens> defaultDatasetLens(Suite const& dataseq) const;

    ReflectionInfos makeReflectionInfos(
        Experiment const&, Reflection const&, uint gmaSlices, Range const&, Progress*) const;

    Ranges const& bgRanges() const { return bgRanges_; }
    uint bgPolyDegree() const { return bgPolyDegree_; }
    bool intenScaledAvg() const { return intenScaledAvg_; }
    preal intenScale() const { return intenScale_; }
    Reflections const& reflections() const { return reflections_; }

    eNorm norm() const { return norm_; }

    qreal calcAvgBackground(Suite const&) const;
    qreal calcAvgBackground(Experiment const&) const;
};

#endif // SESSION_H
