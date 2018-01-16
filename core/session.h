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
#include "core/calc/reflection.h"
#include "core/calc/reflection_info.h"
#include "core/data/angle_map.h"
#include "core/data/datafile.h"
#include "core/data/experiment.h"
#include "core/data/image.h"
#include "core/typ/async.h"
#include "core/typ/cache.h"
#include "core/typ/singleton.h"
#include <QSharedPointer> // no auto rm

extern class Session* gSession;

//! Companion of MainWin and TheHub, holds data and data-related settings.

//! One instance of this class coexists with the main window. It is accessible from everywhere
//! through the global pointer gSession.

//! The original idea was that core and GUI only communicate via function calls between
//! Session and TheHub. In the big refactoring after v2.0.5, this has been given up.

class Session final : public ISingleton<Session> {
public:
    Session();

    // Modifying methods:
    void clear();
    void addGivenFiles(const QStringList& filePaths) THROWS;
    void removeFile(int i);
    void setCorrFile(QSharedPointer<Datafile>) THROWS; // Load or remove a correction file.
    void removeCorrFile();
    void assembleExperiment(const vec<int>, const int);

    void setImageTransformMirror(bool);
    void setImageTransformRotate(ImageTransform const&);
    void setImageCut(bool isTopOrLeft, bool linked, ImageCut const&);
    void setGeometry(qreal detectorDistance, qreal pixSize, IJ const& midPixOffset);
    void setGammaRange(const Range& r) { gammaRange_ = r; }
    void setBgRanges(const Ranges& rr) { bgRanges_ = rr; }
    bool addBgRange(const Range& r) { return bgRanges_.add(r); }
    bool removeBgRange(const Range& r) { return bgRanges_.remove(r); }
    void setBgPolyDegree(int degree) { bgPolyDegree_ = degree; }
    void setIntenScaleAvg(bool, qreal);
    void addReflection(const QString&);
    void addReflection(const QJsonObject& obj);
    void removeReflection(int i) { reflections_.remove(i); }
    void setNorm(eNorm norm) { norm_ = norm; }

    // Const methods:
    int numFiles() const { //!< number of data files (not counting the correction file)
        return files_.count(); }
    const Datafile* file(int i) const { return files_[i].data(); }
    bool hasFile(rcstr fileName) const;
    bool hasCorrFile() const { return !corrFile_.isNull(); }
    const Datafile* corrFile() const { return corrFile_.data(); }
    shp_Image corrImage() const { return corrImage_; }
    const Image* intensCorr() const;
    void tryEnableCorr(bool on) { corrEnabled_ = on && hasCorrFile(); }
    bool isCorrEnabled() const { return corrEnabled_; }

    vec<int> const& filesSelection() const { return filesSelection_; }
    Experiment const& experiment() const { return experiment_; }

    size2d imageSize() const;
    ImageTransform const& imageTransform() const { return imageTransform_; }
    ImageCut const& imageCut() const { return imageCut_; }

    Geometry const& geometry() const { return geometry_; }
    IJ midPix() const;

    const Range& gammaRange() const { return gammaRange_; }

    shp_AngleMap angleMap(Measurement const&) const;
    static shp_AngleMap angleMap(Session const& session, Measurement const& ds) {
        return session.angleMap(ds); }

    shp_ImageLens imageLens(const Image&, bool trans, bool cut) const;
    shp_SequenceLens defaultDataseqLens(Cluster const& dataseq) const;

    ReflectionInfos makeReflectionInfos(
        Reflection const&, int gmaSlices, const Range&, Progress*) const;

    const Ranges& bgRanges() const { return bgRanges_; }
    int bgPolyDegree() const { return bgPolyDegree_; }
    bool intenScaledAvg() const { return intenScaledAvg_; }
    qreal intenScale() const { return intenScale_; }
    Reflections const& reflections() const { return reflections_; }

    qreal calcAvgBackground(Cluster const&) const;
    qreal calcAvgBackground() const;

private:
    QVector<QSharedPointer<Datafile>> files_; //!< data files
    QSharedPointer<Datafile> corrFile_; //!< correction file
    shp_Image corrImage_;
    bool corrEnabled_;
    vec<int> filesSelection_; // from these files
    Experiment experiment_; // cluster collected ...
    bool intenScaledAvg_; // if not, summed
    qreal intenScale_;
    size2d imageSize_; //!< All images must have this same size
    ImageTransform imageTransform_;
    ImageCut imageCut_;
    Geometry geometry_;
    Range gammaRange_;
    int bgPolyDegree_;
    Ranges bgRanges_;
    Reflections reflections_;
    eNorm norm_;

    mutable Image intensCorr_;
    mutable bool corrHasNaNs_;
    mutable cache_lazy<ImageKey, AngleMap> angleMapCache_;

    void updateImageSize(); //!< Clears image size if session has no files
    void setImageSize(size2d const&) THROWS; //!< Ensures same size for all images
    void computeOffsets();

    shp_SequenceLens dataseqLens(Cluster const&, eNorm, bool trans, bool cut) const;
    void calcIntensCorr() const;
    Curve curveMinusBg(SequenceLens const&, const Range&) const;
    ReflectionInfo makeReflectionInfo(SequenceLens const&, Reflection const&, const Range&) const;
};

#endif // SESSION_H
