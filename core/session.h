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

#include "core/calc/active_clusters.h"
#include "core/calc/peak_info.h"
#include "core/data/corrset.h"
#include "core/data/dataset.h"
#include "core/data/gamma_selection.h"
#include "core/data/theta_selection.h"
#include "core/fit/peak_functions.h"
#include "core/pars/baseline.h"
#include "core/pars/params.h"
#include "core/pars/peak.h"
#include "core/raw/angle_map.h"
#include "core/typ/cached.h"

extern class Session* gSession;

//! Companion of MainWin and MainWin, holds data and data-related settings.

//! One instance of this class coexists with the main window. It is accessible from everywhere
//! through the global pointer gSession.

class Session {
public:
    Session();
    Session(const Session&) = delete;

    // modifying methods:
    void clear();
    void sessionFromJson(const QByteArray&);

    void updateImageSize(); //!< Clears image size if session has no files
    void setImageSize(const size2d&); //!< Also ensures same size for all images

    void onDetector();      //!< detector detector has changed
    void onBaseline();      //!< settings for baseline fit have changed
    void onPeaks();         //!< a peak has been added or removed
    void onPeakPars(int);   //!< settings for one peak fit have changed
    void onInterpol();      //!< interpolation control parameters have changed

    // const methods:
    QByteArray serializeSession() const; // TODO rename toJson
    size2d imageSize() const;

    // const abbreviations to member member calls
    bool hasData() const { return dataset.countFiles(); }
    bool hasCorrFile() const { return corrset.hasFile(); }
    const HighlightedData& highlightedCluster() const { return dataset.highlight(); }

    Dataset dataset;
    Corrset corrset;
    Params  params;
    GammaSelection gammaSelection;
    ThetaSelection thetaSelection;
    Baseline baseline;
    Peaks peaks;
    ActiveClusters activeClusters;
    KeyedCache<AngleMap, deg> angleMap;
    AllPeaks allPeaks;

private:
    size2d imageSize_; //!< All images must have this same size
};

#endif // SESSION_H
