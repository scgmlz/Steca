// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/curve.cpp
//! @brief     Implements class Curve
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "typ/curve.h"
#include "def/idiomatic_for.h"

void Curve::clear() {
    xs_.clear();
    ys_.clear();
    rgeX_.invalidate();
    rgeY_.invalidate();
}

bool Curve::isEmpty() const {
    return xs_.isEmpty();
}

uint Curve::count() const {
    debug::ensure(xs_.count() == ys_.count());
    return xs_.count();
}

bool Curve::isOrdered() const {
    return std::is_sorted(xs_.cbegin(), xs_.cend());
}

void Curve::append(qreal x, qreal y) {
    xs_.append(x);
    ys_.append(y);
    rgeX_.extendBy(x);
    rgeY_.extendBy(y);
}

Curve Curve::intersect(Range const& range) const {
    Curve res;

    if (!range.isEmpty()) {
        debug::ensure(isOrdered());

        uint xi = 0, cnt = count();
        auto minX = range.min, maxX = range.max;
        while (xi < cnt && xs_.at(xi) < minX)
            ++xi;
        while (xi < cnt && xs_.at(xi) <= maxX) {
            res.append(xs_.at(xi), ys_.at(xi));
            ++xi;
        }
    }

    return res;
}

Curve Curve::intersect(Ranges const& ranges) const {
    Curve res;

    // collect points that are in ranges
    // it works because both curve points and ranges are ordered and ranges are
    // non-overlapping
    debug::ensure(isOrdered());

    uint xi = 0, cnt = count();
    for_i (ranges.count()) {
        auto& range = ranges.at(i);
        auto minX = range.min, maxX = range.max;
        while (xi < cnt && xs_.at(xi) < minX)
            ++xi;
        while (xi < cnt && xs_.at(xi) <= maxX) {
            res.append(xs_.at(xi), ys_.at(xi));
            ++xi;
        }
    }

    return res;
}

//! Subtracts a background that is given as a funtion y(x).
void Curve::subtract(std::function<qreal(qreal)> const& func)
{
    for_i (count())
        ys_[i] -= func(xs_.at(i));
}

uint Curve::maqpairindex() const {
    if (isEmpty())
        return 0;

    auto yMax = ys_.first();
    uint index = 0;

    for_i (count()) {
        auto y = ys_.at(i);
        if (y > yMax) {
            yMax = y;
            index = i;
        }
    }

    return index;
}

qreal Curve::sumY() const {
    qreal sum = 0;
    for_i (count())
        sum += ys_.at(i);
    return sum;
}
