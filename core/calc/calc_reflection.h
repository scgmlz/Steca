// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/calc/calc_reflection.h
//! @brief     Defines class Reflection
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef CALC_REFLECTION_H
#define CALC_REFLECTION_H

#include "fit/fit_fun.h"

namespace calc {

class Reflection final {
public:
    Reflection(QString const& peakFunctionName = "Raw");

    void from_json(typ::JsonObj const&) THROWS;
    void setPeakFunction(QString const&);
    void setRange(typ::Range const&);
    void invalidateGuesses();
    void setGuessPeak(qpair const& peak) { peakFunction_->setGuessedPeak(peak); }
    void setGuessFWHM(fwhm_t fwhm) { peakFunction_->setGuessedFWHM(fwhm); }
    void fit(typ::Curve const&);

    QString peakFunctionName() const { return peakFunction_->name(); }
    fit::PeakFunction const& peakFunction() const;
    typ::Range const& range() const;
    typ::JsonObj to_json() const;

private:
    scoped<fit::PeakFunction*> peakFunction_;
};

typedef QSharedPointer<Reflection> shp_Reflection;
typedef typ::vec<shp_Reflection> Reflections;

} // namespace calc

Q_DECLARE_METATYPE(calc::shp_Reflection)

#endif // CORE_REFLECTION_H
