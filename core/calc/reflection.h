// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/reflection.h
//! @brief     Defines class Reflection
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef REFLECTION_H
#define REFLECTION_H

#include "core/def/special_pointers.h"
#include "core/fit/fit_fun.h"
#include "core/typ/curve.h"
#include "core/typ/range.h"
#include "core/typ/realpair.h"
#include "core/typ/types.h" // for fwhm_t
#include <QSharedPointer> // no auto rm

//! Wraps a PeakFunction (pimpl idiom)

class Reflection final {
public:
    Reflection(const QString& peakFunctionName = "Raw");

    void from_json(JsonObj const&) THROWS;
    void setPeakFunction(const QString&);
    void setRange(const Range&);
    void invalidateGuesses();
    void setGuessPeak(qpair const& peak);
    void setGuessFWHM(fwhm_t fwhm);
    void fit(Curve const&);

    QString peakFunctionName() const;
    PeakFunction const& peakFunction() const;
    const Range& range() const;
    JsonObj to_json() const;

private:
    scoped<PeakFunction*> peakFunction_; //!< pimpl (pointer to implementation)
};

typedef QSharedPointer<Reflection> shp_Reflection;
typedef vec<shp_Reflection> Reflections;

Q_DECLARE_METATYPE(shp_Reflection)

#endif // REFLECTION_H
