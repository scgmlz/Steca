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
#include "core/data/gamma_selection.h"
#include "core/pars/params.h"
#include "core/calc/interpol_params.h"
#include "core/calc/peak.h"
#include "core/data/theta_selection.h"
#include "core/data/corrset.h"
#include "core/data/dataset.h"
#include "core/data/peak_info.h"
#include "core/raw/angle_map.h"
#include "core/pars/detector.h"
#include "core/typ/cached.h"

extern class Session* gSession;

//! Companion of MainWin and MainWin, holds data and data-related settings.

//! One instance of this class coexists with the main window. It is accessible from everywhere
//! through the global pointer gSession.

class Session : public QObject {
    Q_OBJECT
public:
    Session();
    ~Session();
    Session(const Session&) = delete;

    // accessor methods:
    Peaks& peaks() { return peaks_; }
    const Peaks& peaks() const { return peaks_; }

    Baseline& baseline() { return baseline_; }
    const Baseline& baseline() const { return baseline_; }

    GammaSelection& gammaSelection() { return gammaSelection_; }
    const GammaSelection& gammaSelection() const { return gammaSelection_; }

    ThetaSelection& thetaSelection() { return thetaSelection_; }
    const ThetaSelection& thetaSelection() const { return thetaSelection_; }

    InterpolParams& interpol() { return interpolParams_; }
    const InterpolParams& interpol() const { return interpolParams_; }

    const AngleMap& angleMap(const deg tth) { return angleMap_.get(tth); }

    const PeakInfos& directPeakInfos() const { return directPeakInfos_; }
    const PeakInfos& interpolatedPeakInfos() const { return interpolatedPeakInfos_; }
    const PeakInfos& peakInfos() const;

    void setMetaSelected(int, bool);
    bool metaIsSelected(int i) const { return metaSelection_[i]; }
    int metaSelectedCount() const { return metaInfoNums_.size(); }
    int metaSelectedAt(int i) const { return metaInfoNums_.at(i); }

    // modifying methods:
    void clear();
    void sessionFromJson(const QByteArray&);

    void setDirectPeakInfos(PeakInfos&& val) {
        directPeakInfos_ = std::move(val); }
    void setInterpolatedPeakInfos(PeakInfos&& val) {
        interpolatedPeakInfos_ = std::move(val); }

    void updateImageSize(); //!< Clears image size if session has no files
    void setImageSize(const size2d&); //!< Also ensures same size for all images

    // const methods:
    QByteArray serializeSession() const;

    size2d imageSize() const;

    // const abbreviations to member member calls
    bool hasData() const { return dataset.countFiles(); }
    bool hasCorrFile() const { return corrset.hasFile(); }
    const ActiveClusters& activeClusters() const { return dataset.activeClusters(); }

    Params  params;
    Dataset dataset;
    Corrset corrset;

signals:
    void sigDetector();      //!< detector detector has changed

private:
    // with reference accessor methods:
    size2d imageSize_; //!< All images must have this same size
    Peaks peaks_;
    Baseline baseline_;
    GammaSelection gammaSelection_;
    ThetaSelection thetaSelection_;
    InterpolParams interpolParams_;
    PeakInfos directPeakInfos_;
    PeakInfos interpolatedPeakInfos_;
    // others
    std::vector<bool> metaSelection_; //!< true if meta datum is to be displayed
    std::vector<int> metaInfoNums_; //!< indices of metadata items selected for display
    KeyedCache<AngleMap, deg> angleMap_;
};

#endif // SESSION_H
