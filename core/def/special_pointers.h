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
