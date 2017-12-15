// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/def/special_pointers.h
//! @brief     Defines and implements templated class scoped
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef SPECIAL_POINTERS_H
#define SPECIAL_POINTERS_H

#include "def/debug.h"
#include <cstddef> // for size_t
#include <type_traits> // for is_assignable

// pointers that cannot be null

template <class P> class not_null {
    static_assert(std::is_assignable<P&, std::nullptr_t>::value, "no nullptr");

public:
    static not_null from(P p) { return not_null(p); }

    not_null(not_null const&) = default;
    not_null& operator=(not_null const&) = default;

    // from another not_null
    template <typename O, typename Dummy = std::enable_if<std::is_convertible<O, P>::value>>
    not_null(not_null<O> const& that) {
        *this = that;
    }

    // from another not_null
    template <typename O, typename Dummy = std::enable_if<std::is_convertible<O, P>::value>>
    not_null& operator=(not_null<O> const& that) {
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

    explicit not_null(P p) : p_(p) { debug::ensure(p_); }

    // no pointer arithmetics
    not_null<P>& operator++() = delete;
    not_null<P>& operator--() = delete;
    not_null<P> operator++(int) = delete;
    not_null<P> operator--(int) = delete;
    not_null<P>& operator+(size_t) = delete;
    not_null<P>& operator+=(size_t) = delete;
    not_null<P>& operator-(size_t) = delete;
    not_null<P>& operator-=(size_t) = delete;
};

//! scoped pointer that auto-deletes what he has

template <class P> class scoped final {
public:
    scoped(P ptr) : p_(ptr) {}

    ~scoped() { reset(nullptr); }

    bool isNull() const { return !p_; }

    void reset(P ptr) {
        delete p_;
        p_ = ptr;
    }

    P take() {
        auto p = p_;
        p_ = nullptr;
        return p;
    }

    P ptr() const { return p_; }
    operator P() const { return p_; }
    P operator->() const { return p_; }

private:
    P p_;
};

#endif // SPECIAL_POINTERS_H
