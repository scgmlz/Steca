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

#include <QJsonArray>
#include <vector> // no auto rm

class JsonObj;

//! a range of values - a closed interval
class Range {
public:
    Range(); //!< invalid (NaN)
    Range(double min, double max);
    Range(const std::vector<double>&);

    static Range infinite(); //!< factory: -inf .. +inf
    static Range safeFrom(double, double); //!< safe factory

    void fromJson(const JsonObj&);
    void invalidate(); //!< make invalid
    void extendBy(double); //!< extend to include the number
    void extendBy(const Range&); //!< extend to include the range
    void set(double min, double max); //!< must be: min <= max
    void setOne(double val, bool namelyMax); //!< sets either min or max

    QJsonObject toJson() const;
    bool isValid() const; //!< is not NaN
    bool isEmpty() const; //!< is invalid or empty
    double width() const;
    double center() const;
    Range slice(int i, int n) const;
    bool contains(double) const;
    bool contains(const Range&) const;
    bool intersects(const Range&) const;
    Range intersect(const Range&) const;
    QString to_s(const QString& unit="", int precision=5, int digitsAfter=2) const;

    double min, max; // this is the range
};

//! A *sorted* list of *non-overlapping* ranges, of which one is always selected.
class Ranges {
public:
    void fromJson(const QJsonArray&);
    void clear();
    void add(const Range&);
    void removeSelected();
    void select(int i);
    bool selectByValue(double x);

    Range* selectedRange() { return isEmpty() ? nullptr : &ranges_.at(selected_); }

    QJsonArray toJson() const;
    bool isEmpty() const { return ranges_.empty(); }
    int size() const { return ranges_.size(); }
    const Range& at(int i) const { return ranges_[i]; }
    int selectedIndex() const { return selected_; }

private:
    void sort();
    std::vector<Range> ranges_;
    int selected_;
};

#endif // RANGE_H
