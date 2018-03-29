//  ***********************************************************************************************
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
//  ***********************************************************************************************

#ifndef SESSION_H
#define SESSION_H

#include "core/calc/baseline.h"
#include "core/calc/interpol_params.h"
#include "core/calc/gamma_selection.h"
#include "core/calc/theta_selection.h"
#include "core/calc/lens.h"
#include "core/calc/peak.h"
#include "core/data/peak_info.h"
#include "core/data/corrset.h"
#include "core/data/dataset.h"
#include "core/data/image_transform.h"
#include <iostream>

#define EMIT(sig) std::cerr<<"emitting "<<#sig<<"\n"; emit sig;

extern class Session* gSession;

enum class eNorm {
    NONE,
    MONITOR,
    DELTA_MONITOR,
    DELTA_TIME,
    BACKGROUND,
};

//! Companion of MainWin and MainWin, holds data and data-related settings.

//! One instance of this class coexists with the main window. It is accessible from everywhere
//! through the global pointer gSession.

class Session : public QObject {
    Q_OBJECT
public:
    Session();
    ~Session();

    // accessor methods:
    Dataset& dataset() { return dataset_; }
    const Dataset& dataset() const { return dataset_; }

    Corrset& corrset() { return corrset_; }
    const Corrset& corrset() const { return corrset_; }

    Peaks& peaks() { return peaks_; }
    const Peaks& peaks() const { return peaks_; }

    Baseline& baseline() { return baseline_; }
    const Baseline& baseline() const { return baseline_; }

    Geometry& geometry() { return geometry_; }
    const Geometry& geometry() const { return geometry_; }

    ImageCut& imageCut() { return imageCut_; }
    const ImageCut& imageCut() const { return imageCut_; }

    const ImageTransform& imageTransform() const { return imageTransform_; }

    GammaSelection& gammaSelection() { return gammaSelection_; }
    const GammaSelection& gammaSelection() const { return gammaSelection_; }

    ThetaSelection& thetaSelection() { return thetaSelection_; }
    const ThetaSelection& thetaSelection() const { return thetaSelection_; }

    InterpolParams& interpol() { return interpolParams_; }
    const InterpolParams& interpol() const { return interpolParams_; }

    PeakInfos& peakInfos() { return peakInfos_; }
    const PeakInfos& peakInfos() const { return peakInfos_; }

    void setNormMode(eNorm);
    eNorm normMode() const { return normMode_; }

    // modifying methods:
    void clear();
    void sessionFromJson(const QByteArray&);

    void setMetaSelected(int, bool);

    void setImageTransformMirror(bool);
    void setImageTransformRotate(const ImageTransform&);
    void setIntenScaleAvg(bool, double);
    void updateImageSize(); //!< Clears image size if session has no files
    void setImageSize(const size2d&); //!< Ensures same size for all images

    // const methods:
    QByteArray serializeSession() const;

    bool intenScaledAvg() const { return intenScaledAvg_; }
    double intenScale() const { return intenScale_; }
    bool metaSelected(int i) const { return metaSelection_[i]; }

    bool hasData() const { return dataset().countFiles(); }
    bool hasCorrFile() const { return corrset().hasFile(); }
    const ActiveClusters& activeClusters() const { return dataset().activeClusters(); }

    size2d imageSize() const;
    IJ midPix() const;
    shp_AngleMap angleMap(const Measurement&) const;
    double calcAvgBackground(const Sequence&) const;
    double calcAvgBackground() const;

signals:
    void sigActivated();     //!< selection of active clusters has changed
    void sigBaseline();      //!< baseline settings have changed
    void sigBigtableCols();  //!< column selection in bigtable has changed
    void sigClusters();      //!< list of clusters has changed
    void sigCorr();          //!< corr file has been loaded or unloaded or enabled or disabled
    void sigDataHighlight(); //!< highlighted File or/and Cluster has changed
    void sigDetector();      //!< detector geometry has changed
    void sigDfgram(); //!< diffractogram must be repainted
    void sigFiles();         //!< list of loaded files has changed
    void sigGamma();         //!< gamma selection has changed
    void sigImage();         //!< image must be repainted
    void sigInterpol();      //!< interpolation parameters have changed
    void sigMetaSelection(); //!< meta data selected for display have changed
    void sigNorm();          //!< normalization has changed
    void sigPeaks();         //!< list of peaks or selected peak has changed
    void sigRawFits();       //!< fit results have changed
    void sigTheta();         //!< theta selection has changed

private:
    // with reference accessor methods:
    Dataset dataset_;
    Corrset corrset_;
    Peaks peaks_;
    Baseline baseline_;
    Geometry geometry_;
    ImageCut imageCut_;
    ImageTransform imageTransform_;
    GammaSelection gammaSelection_;
    ThetaSelection thetaSelection_;
    InterpolParams interpolParams_;
    PeakInfos peakInfos_;
    eNorm normMode_ {eNorm::NONE};
    // others
    bool intenScaledAvg_ {true}; // if not, summed
    double intenScale_ {1};
    std::vector<bool> metaSelection_; //!< true if meta datum is to be displayed
    size2d imageSize_; //!< All images must have this same size
    // deeply private:
    mutable cache_lazy<ImageKey, AngleMap> angleMapCache_ {360};
};

#endif // SESSION_H
