// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/def/scoped.h
//! @brief     Defines and implements templated class scoped
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef SCOPED_H
#define SCOPED_H

#include "def/def_debug.h"
#include <type_traits> // for is_assignable
#include <cstddef> // for size_t

// pointers that cannot be null

template <class P> class not_null {
    static_assert(std::is_assignable<P&, std::nullptr_t>::value, "no nullptr");
    private:
    explicit not_null(P p) : p_(p) { EXPECT(p_) }

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

// to mark owning pointers (just a hint)

template <class P> using owner = P;
template <class P> using owner_not_null = not_null<P>;

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

    owner<P> take() {
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

#endif // SCOPED_H
