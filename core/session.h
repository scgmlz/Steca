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

#include "core/pars/baseline.h"
#include "core/data/gamma_selection.h"
#include "core/pars/params.h"
#include "core/pars/peak.h"
#include "core/data/theta_selection.h"
#include "core/data/corrset.h"
#include "core/data/dataset.h"
#include "core/data/peak_info.h"
#include "core/fit/peak_functions.h"
#include "core/raw/angle_map.h"
#include "core/typ/cached.h"

extern class Session* gSession;

//! Companion of MainWin and MainWin, holds data and data-related settings.

//! One instance of this class coexists with the main window. It is accessible from everywhere
//! through the global pointer gSession.

class Session : public QObject {
    Q_OBJECT
public:
    Session();
    Session(const Session&) = delete;

    // accessor methods:
    const PeakInfos& directPeakInfos() const { return directPeakInfos_; }
    const PeakInfos& interpolatedPeakInfos() const { return interpolatedPeakInfos_; }
    const PeakInfos& peakInfos() const;

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
    const HighlightedData& highlightedCluster() const { return dataset.highlight(); }

    Dataset dataset;
    Corrset corrset;
    ActiveClusters activeClusters;
    Params  params;
    KeyedCache<AngleMap, deg> angleMap;
    GammaSelection gammaSelection;
    ThetaSelection thetaSelection;
    Baseline baseline;
    Peaks peaks;

signals:
    void sigDetector();      //!< detector detector has changed
    void sigBaseline();      //!< settings for baseline fit have changed
    void sigPeaks();         //!< a peak has been added or removed
    void sigPeakPars(int);   //!< settings for one peak fit have changed

private:
    // with reference accessor methods:
    size2d imageSize_; //!< All images must have this same size
    PeakInfos directPeakInfos_;
    PeakInfos interpolatedPeakInfos_;
    // others
};

#endif // SESSION_H
