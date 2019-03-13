//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/peakfit/outcome.h
//! @brief     Defines class PeakOutcome (header-only)
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef OUTCOME_H
#define OUTCOME_H

#include <QString>
#include <map>

//! Numeric outcome of peak fit.

class PeakOutcome : public std::map<QString,double> {
public:
    PeakOutcome() {}
    bool has(const QString& key) const { return find(key)!=end(); }
private:
};

#endif // OUTCOME_H
