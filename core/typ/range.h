//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/range.h
//! @brief     Defines classes Range and Ranges
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef RANGE_H
#define RANGE_H

#include "core/def/comparable.h"
#include "core/typ/exception.h"
#include <QJsonArray>
#include <QtGlobal>
#include <vector> // no auto rm

class JsonObj;

//! a range of values - a closed interval
class Range {
public:
    Range(); //!< invalid (NaN)
    Range(double min, double max);
    Range(const std::vector<double>&);

    static Range infinite(); //!< factory: -inf .. +inf

    COMPARABLE(const Range&)

    void invalidate(); //!< make invalid
    bool isValid() const; //!< is not NaN
    bool isEmpty() const; //!< is invalid or empty

    double width() const;
    double center() const;
    Range slice(int i, int n) const;

    double min, max; // this is the range

    void set(double min, double max); //!< must be: min <= max
    void safeSet(double, double); //!< will be set in the right order min/max

    static Range safeFrom(double, double); //!< safe factory

    void extendBy(double); //!< extend to include the number
    void extendBy(const Range&); //!< extend to include the range

    bool contains(double) const;
    bool contains(const Range&) const;
    bool intersects(const Range&) const;
    Range intersect(const Range&) const;

    double bound(double) const; //!< limit the number to the interval, as qBound would

    QJsonObject toJson() const;
    void fromJson(const JsonObj&);

    QString to_s(const QString& unit="", int precision=5, int digitsAfter=2) const;
};

//! A set of *sorted* *non-overlapping* ranges
class Ranges {
public:
    Ranges() {}

    void clear() { ranges_.clear(); }
    bool isEmpty() const { return ranges_.empty(); }
    int count() const { return ranges_.size(); }

    const Range& at(int i) const { return ranges_[i]; }

    bool add(const Range&); //!< collapses overlapping ranges; returns true if *this changed
    bool remove(const Range&); //!< removes (cuts out) a range; returns whether there was a change

    QJsonArray toJson() const;
    void fromJson(const QJsonArray&);

private:
    void sort();
    std::vector<Range> ranges_;
};

#endif // RANGE_H
