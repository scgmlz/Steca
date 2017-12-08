// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/typ_array2d.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TYP_ARRAY2D_H
#define TYP_ARRAY2D_H

#include "def/def_cmp.h"
#include "def/def_gsl.h" // for uint

namespace typ {

struct size2d {
    uint w, h;

    size2d() : size2d(0, 0) {}
    size2d(uint w_, uint h_) : w(w_), h(h_) {}

    bool isEmpty() const { return 0 == w && 0 == h; }

    uint count() const { return w * h; }

    COMPARABLE(size2d const&)

    friend size2d operator-(size2d const& s1, size2d const& s2) {
        int w = to_i(s1.w) - to_i(s2.w);
        int h = to_i(s1.h) - to_i(s2.h);
        return size2d(to_u(qMax(w, 0)), to_u(qMax(h, 0)));
    }

    size2d transposed() const { return size2d(h, w); }
};

// 2D (indexed by uint i/j) array
template <typename T> class Array2D {
    private:
    size2d size_;

    typedef T* col_t;
    col_t* ts_;

    void alloc(size2d const& size) {
        free();

        if (!(size_ = size).isEmpty()) {
            ts_ = static_cast<col_t*>(::calloc(size_.w, sizeof(col_t*)));
            for (uint i = 0; i < size_.w; ++i)
                ts_[i] = static_cast<col_t>(::calloc(size_.h, sizeof(T)));
        }
    }

    void free() {
        if (ts_) {
            for (uint i = 0; i < size_.w; ++i)
                ::free(ts_[i]);
            ::free(ts_);
            ts_ = nullptr;
        }
    }

public:
    // empty array
    Array2D() : size_(0, 0), ts_(nullptr) {}

    Array2D(Array2D const&) = delete;
    Array2D& operator=(Array2D const&) = delete;

    virtual ~Array2D() { free(); }

    // is empty?
    bool isEmpty() const { return size_.isEmpty(); }

    // 2D image size
    size2d const& size() const { return size_; }

    // make empty
    void clear() { alloc(size2d(0, 0)); }

    // allocate and fill with a value
    void fill(T const& val, size2d const& size) {
        alloc(size);
        for (uint i = 0; i < size_.w; ++i)
            for (uint j = 0; j < size_.h; ++j)
                ts_[i][j] = val;
    }

    // allocate and fill with a default value
    void resize(size2d const& size) { fill(T(), size); }

    // access using 1D index
    T const& at(uint i) const { return at(i % size_.w, i / size_.h); }

    // access using 2D index
    T const& at(uint i, uint j) const {
        EXPECT(i < size_.w && j < size_.h)
        return ts_[i][j];
    }

    // set using 1D index
    void setAt(uint i, T const& val) { setAt(i % size_.w, i / size_.h, val); }

    // set using 2D index
    void setAt(uint i, uint j, T const& val) {
        EXPECT(i < size_.w && j < size_.h)
        ts_[i][j] = val;
    }

    // mutable access
    T& refAt(uint i, uint j) const {
        EXPECT(i < size_.w && j < size_.h)
        return ts_[i][j];
    }
};

} // namespace typ

#endif // TYP_ARRAY2D_H
