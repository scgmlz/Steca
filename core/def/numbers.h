// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/def/numbers.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

// adapted parts of https://github.com/Microsoft/GSL.git
// added some more

#ifndef NUMBERS_H
#define NUMBERS_H

#include "def/debug.h"
#include "def/macros.h"
#include <QtGlobal> // to define Q_OS_WIN

// casting signed <-> unsigned

#if defined(Q_OS_WIN) || defined(Q_OS_OSX)
// do not handle the below templates

inline int to_i(unsigned int u) { return u; }

inline unsigned int to_u(int i) { return i; }

inline unsigned int clip_u(int i) { return qMax(0, i); }

#else

#ifndef QT_NO_DEBUG
#endif

// unsigned to signed
template <typename T> typename std::__make_signed<T>::__type to_i(T t) {
    static_assert(std::is_unsigned<T>::value, "to_i(signed)");
#ifndef QT_NO_DEBUG
    auto max = std::numeric_limits<typename std::__make_signed<T>::__type>::max();
    debug::ensure(static_cast<T>(max) >= t, "to_i(too big)");
#endif
    return typename std::__make_signed<T>::__type(t);
}

// signed to unsigned
template <typename T> typename std::__make_unsigned<T>::__type to_u(T t) {
    static_assert(std::is_signed<T>::value, "to_u(signed)");
    debug::ensure(0 <= t, "to_u(attempt to convert a negative value)");
    return typename std::__make_unsigned<T>::__type(t);
}

template <typename T> typename std::__make_unsigned<T>::__type clip_u(T t) {
    static_assert(std::is_signed<T>::value, "clip_u(signed)");
    return typename std::__make_unsigned<T>::__type(qMax(0, t));
}

#endif

// natural numbers 1...

#ifndef QT_NO_DEBUG

class pint {
public:
    explicit pint(uint val) : val_(val) { debug::ensure(1 <= val); }

    explicit pint(int val) : pint(to_u(val)) {}

    operator uint() const { return val_; }

private:
    uint val_;
};

#else

typedef uint pint;

#endif

// reals

#undef NAN
#undef INF

extern qreal const NAN; // silent nan
extern qreal const INF;

// positive reals

#ifndef QT_NO_DEBUG

class preal {
public:
    explicit preal(qreal val) : val_(val) { debug::ensure(val>0); }

    operator qreal() const { return val_; }

private:
    qreal val_;
};

#else

typedef qreal preal;

#endif

#endif // NUMBERS_H
