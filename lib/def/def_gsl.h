// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      lib/def/def_gsl.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

// adapted parts of https://github.com/Microsoft/GSL.git
// added some more

#ifndef DEF_GSL_H
#define DEF_GSL_H

#include "def/def_debug.h"
#include "def/def_macros.h"

// pointers that cannot be null

template <class P> class not_null {
  static_assert(std::is_assignable<P&, std::nullptr_t>::value, "no nullptr");
  CLASS(not_null)
private:
  explicit not_null(P p) : p_(p) { EXPECT(p_) }

public:
  static not_null from(P p) { return not_null(p); }

  not_null(rc)      = default;
  not_null& operator=(rc) = default;

  // from another not_null
  template <typename O,
            typename Dummy = std::enable_if<std::is_convertible<O, P>::value>>
  not_null(not_null<O> const& that) {
    *this = that;
  }

  // from another not_null
  template <typename O,
            typename Dummy = std::enable_if<std::is_convertible<O, P>::value>>
  not_null& operator       =(not_null<O> const& that) {
    p_ = that.ptr();
    return *this;
  }

  P ptr() const { return p_; }
  operator P() const { return p_; }
  P operator->() const { return p_; }

  bool operator==(P const& p) const { return p_ == p; }
  bool operator!=(P const& p) const { return p_ != p; }

private:
  P p_;

  // no pointer arithmetics
  not_null<P>& operator++()       = delete;
  not_null<P>& operator--()       = delete;
  not_null<P> operator++(int)     = delete;
  not_null<P> operator--(int)     = delete;
  not_null<P>& operator+(size_t)  = delete;
  not_null<P>& operator+=(size_t) = delete;
  not_null<P>& operator-(size_t)  = delete;
  not_null<P>& operator-=(size_t) = delete;
};

// to mark owning pointers (just a hint)

template <class P> using owner          = P;
template <class P> using owner_not_null = not_null<P>;

// scoped pointer that auto-deletes what he has

template <class P> class scoped final {
public:
  scoped(P ptr) : p_(ptr) {}

  ~scoped() { reset(nullptr); }

  bool isNull() const { return !p_; }

  void reset(P ptr) {
    delete p_;
    p_ = ptr;
  }

  owner<P> take() {
    auto p = p_;
    p_     = nullptr;
    return p;
  }

  P ptr() const { return p_; }
  operator P() const { return p_; }
  P operator->() const { return p_; }

private:
  P p_;
};

// casting signed <-> unsigned

#if defined(Q_OS_WIN) || defined(Q_OS_OSX)
// do not handle the below templates

inline int to_i(unsigned int u) {
  return u;
}

inline unsigned int to_u(int i) {
  return i;
}

inline unsigned int clip_u(int i) {
  return qMax(0, i);
}

#else

#ifndef QT_NO_DEBUG
#include <limits>
#endif

// unsigned to signed
template <typename T> typename std::__make_signed<T>::__type to_i(T t) {
  static_assert(std::is_unsigned<T>::value, "to_i(signed)");
#ifndef QT_NO_DEBUG
  auto max = std::numeric_limits<typename std::__make_signed<T>::__type>::max();
  EXPECT2(static_cast<T>(max) >= t, "to_i(too big)")
#endif
  return typename std::__make_signed<T>::__type(t);
}

// signed to unsigned
template <typename T> typename std::__make_unsigned<T>::__type to_u(T t) {
  static_assert(std::is_signed<T>::value, "to_u(signed)");
  EXPECT2(0 <= t, "to_u(attempt to convert a negative value)")
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
  explicit pint(uint val) : val_(val) { EXPECT(1 <= val) }

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

extern qreal const NAN;  // silent nan
extern qreal const INF;

// positive reals

#ifndef QT_NO_DEBUG

class preal {
public:
  explicit preal(qreal val) : val_(val) { EXPECT(0 < val) }

  operator qreal() const { return val_; }

private:
  qreal val_;
};

#else

typedef qreal preal;

#endif

#endif  // DEF_GSL
