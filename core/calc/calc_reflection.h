// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/calc/calc_reflection.h
//! @brief     Defines class Reflection
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef CALC_REFLECTION_H
#define CALC_REFLECTION_H

#include "def/special_pointers.h"
#include "typ/curve.h"
#include "typ/realpair.h"
#include "typ/types.h"
#include <QSharedPointer> // no auto rm

class PeakFunction;

class Reflection final {
public:
    Reflection(QString const& peakFunctionName = "Raw");

    void from_json(JsonObj const&) THROWS;
    void setPeakFunction(QString const&);
    void setRange(Range const&);
    void invalidateGuesses();
    void setGuessPeak(qpair const& peak);
    void setGuessFWHM(fwhm_t fwhm);
    void fit(Curve const&);

    QString peakFunctionName() const;
    PeakFunction const& peakFunction() const;
    Range const& range() const;
    JsonObj to_json() const;

private:
    scoped<PeakFunction*> peakFunction_;
};

typedef QSharedPointer<Reflection> shp_Reflection;
typedef vec<shp_Reflection> Reflections;

Q_DECLARE_METATYPE(shp_Reflection)

#endif // CALC_REFLECTION_H
