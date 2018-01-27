// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/baseline.h
//! @brief     Defines class Baseline
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef BASELINE_H
#define BASELINE_H

#include "core/def/special_pointers.h"
#include "core/fit/fit_fun.h"
#include "core/typ/curve.h"
#include "core/typ/range.h"
#include "core/typ/realpair.h"
#include "core/typ/types.h" // for fwhm_t
#include <QSharedPointer> // no auto rm
#include <QStringList>

class Baseline {
public:
    void fromJson(const JsonObj obj);
    void clear();
    void setRanges(const Ranges&);
    void addRange(const Range&);
    void removeRange(const Range&);
    void setPolynomDegree(int);

    QJsonObject toJson() const;

    const Ranges& ranges() const { return ranges_; }
    int polynomDegree() const { return polynomDegree_; }

private:
    int polynomDegree_ {0};
    Ranges ranges_;
};

#endif // BASELINE_H
