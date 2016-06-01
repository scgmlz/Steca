// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_gsl.h
//! @brief     Selections from Guidelines Support Library
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Antti Soininen, Jan Burle, Rebecca Brydon
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef CORE_GSL_H
#define CORE_GSL_H

#include "core_debug.h"

// adapted parts of https://github.com/Microsoft/GSL.git

/// to mark owning pointers
template <class P> using owner = P;

/// pointers that cannot be null
template <class P> class not_null {
  static_assert(std::is_assignable<P&, std::nullptr_t>::value,
                "P cannot be assigned nullptr.");
private:
  explicit not_null(P p) : ptr_(p) { EXPECT(ptr_) }

public:
  static not_null from(P p) {
    return not_null(p);
  }

  not_null(not_null const&)             = default;
  not_null& operator=(not_null const&)  = default;

  /// from another not_null
  template <typename O,
            typename Dummy = std::enable_if<std::is_convertible<O, P>::value>>
  not_null(not_null<O> const& that) {
    *this = that;
  }

  /// from another not_null
  template <typename O,
            typename Dummy = std::enable_if<std::is_convertible<O, P>::value>>
  not_null& operator =(not_null<O> const& that) {
    ptr_ = that.ptr();
    return *this;
  }

  P ptr() const { return ptr_; }

  operator P()   const { return ptr(); }
  P operator->() const { return ptr(); }

  bool operator==(const P& that) const { return ptr_ == that; }
  bool operator!=(const P& that) const { return ptr_ != that; }

private:
  P ptr_;

  // no pointer arithmetics
  not_null<P>& operator++()       = delete;
  not_null<P>& operator--()       = delete;
  not_null<P>  operator++(int)    = delete;
  not_null<P>  operator--(int)    = delete;
  not_null<P>& operator+ (size_t) = delete;
  not_null<P>& operator+=(size_t) = delete;
  not_null<P>& operator- (size_t) = delete;
  not_null<P>& operator-=(size_t) = delete;
};

#endif  // CORE_GSL_H
