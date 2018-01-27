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
#include <QStringList>

//! Wraps a PeakFunction (pimpl idiom)

class Reflection {
public:
    Reflection(const QString& functionName = "Raw");

    static Reflection* from_json(JsonObj const&) THROWS;

    void setPeakFunction(const QString&);
    void setRange(const Range&);
    void invalidateGuesses();
    void setGuessPeak(qpair const& peak);
    void setGuessFWHM(fwhm_t fwhm);
    void fit(Curve const&);

    QString functionName() const;
    PeakFunction const& peakFunction() const;
    const Range& range() const;
    JsonObj to_json() const;

private:
    scoped<PeakFunction*> peakFunction_; //!< pimpl (pointer to implementation)
};


//! All user defined peaks, of which one is selected to be acted on by default.

class Peaks {
public:
    void clear();
    void add(const QString&);
    void add(const QJsonObject& obj);
    void remove(int i);
    void select(Reflection* reflection);

    const Reflection& at(int i) const { return *reflections_.at(i); }
    Reflection& at(int i) { return *reflections_.at(i); } // used only once

    int count() const { return reflections_.size(); }
    QStringList names() const;
    QJsonArray toJson() const;

    Reflection* selected_ {nullptr};
    std::vector<Reflection*> reflections_;
private:
};

#endif // REFLECTION_H
