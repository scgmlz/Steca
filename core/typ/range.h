// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/range.h
//! @brief     Defines classes Range and Ranges.
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef RANGE_H
#define RANGE_H

#include "def/comparable.h"
#include "def/numbers.h"
#include "typ/exception.h"
#include "typ/vec.h"
#include <QJsonArray>

namespace typ {

class JsonObj;

//! a range of values - a closed interval
class Range {
public:
    Range(); //!< invalid (NaN)
    Range(qreal min, qreal max); //!< normal

    static Range infinite(); //!< factory: -inf .. +inf

    COMPARABLE(Range const&)

    void invalidate(); //!< make invalid
    bool isValid() const; //!< is not NaN
    bool isEmpty() const; //!< not valid or empty

    qreal width() const;
    qreal center() const;

    qreal min, max; // this is the range

    void set(qreal min, qreal max); //!< must be: min <= max
    void safeSet(qreal, qreal); //!< will be set in the right order min/max

    static Range safeFrom(qreal, qreal); //!< safe factory

    void extendBy(qreal); //!< extend to include the number
    void extendBy(Range const&); //!< extend to include the range

    bool contains(qreal) const;
    bool contains(Range const&) const;
    bool intersects(Range const&) const;
    Range intersect(Range const&) const;

    qreal bound(qreal) const; //!< limit the number to the interval, as qBound would

    JsonObj to_json() const;
    void from_json(JsonObj const&) THROWS;
};

//! A set of *sorted* *non-overlapping* ranges
class Ranges {
public:
    Ranges() {}

    void clear() { ranges_.clear(); }
    bool isEmpty() const { return ranges_.isEmpty(); }
    uint count() const { return ranges_.count(); }

    Range const& at(uint i) const { return ranges_.at(i); }

    bool add(Range const&); //!< collapses overlapping ranges; returns true if *this changed

    bool rem(Range const&); //!< removes (cuts out) a range; returns whether there was a change

    QJsonArray to_json() const;
    void from_json(QJsonArray const&) THROWS;

private:
    void sort();
    vec<Range> ranges_;
};

} // namespace typ

#endif // RANGE_H
