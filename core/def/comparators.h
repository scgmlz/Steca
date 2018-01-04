// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/def/comparators.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef COMPARATORS_H
#define COMPARATORS_H

// a helper for int compare()
#define RET_COMPARE_VALUE(val)                                                    \
    if (val < that.val)                                                           \
        return -1;                                                                \
    if (val > that.val)                                                           \
        return +1;

#define RET_COMPARE_VALUE2(v1, v2)                                                \
    if (v1 < v2)                                                                  \
        return -1;                                                                \
    if (v1 > v2)                                                                  \
        return +1;

#define RET_COMPARE_COMPARABLE(o)                                                 \
    for (int cmp = o.compare(that.o); cmp;)                                       \
        return cmp;

#define EQ_NE_OPERATOR(T)                                                         \
    bool T::operator==(T const& that) const { return 0 == compare(that); }        \
    bool T::operator!=(T const& that) const { return 0 != compare(that); }

#define VALID_EQ_NE_OPERATOR(T)                                                   \
    bool T::operator==(T const& that) const {                                     \
        return isValid() && that.isValid() && 0 == compare(that);                 \
    }                                                                             \
    bool T::operator!=(T const& that) const {                                     \
       return isValid() && that.isValid() && 0 != compare(that); }

#endif // COMPARATORS_H
