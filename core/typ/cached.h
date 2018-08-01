//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/cached.h
//! @brief     Defines class templates Cached, KeyedCache, CachingVector
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef CACHED_H
#define CACHED_H

#include "qcr/base/debug.h" // TMP
#include <functional>
#include <memory>
#include <vector>

//! Cached object. Simple version.
template<typename T>
class Cached {
public:
    Cached() = delete;
    Cached(std::function<T(void)> f) : remake_(f) {}
    Cached(const Cached&) = delete;
    void invalidate() const { cached_.release(); }
    const T& get() const {
        if (!cached_)
            cached_.reset( std::move(new T{remake_()}) );
        return *cached_;
    }
private:
    mutable std::unique_ptr<T> cached_;
    const std::function<T(void)> remake_;
};


//! Cached object. Experimental version.
template<typename Parent, typename T>
class Kached {
public:
    Kached() = delete;
    Kached(std::function<T(const Parent*)> f) : remake_(f) {}
    Kached(const Kached&) = delete;
    Kached(Kached&&) = default;
    void invalidate() const { cached_.release(); }
    const T& get(const Parent* parent) const {
        if (!cached_)
            cached_.reset( std::move(new T{remake_(parent)}) );
        return *cached_;
    }
private:
    mutable std::unique_ptr<T> cached_;
    const std::function<T(const Parent*)> remake_;
};


//! Cached object with key.
template<typename T, typename K>
class KeyedCache {
public:
    void invalidate() const { cached_.release(); }
    const T& get(const K key) const {
        if (!cached_ || key!=key_) {
            cached_.reset( std::move(new T(key)) );
            key_ = key;
        }
        return *cached_;
    }
private:
    mutable std::unique_ptr<T> cached_;
    mutable K key_;
};


//! Vector of individually cached objects.
template<typename Owner, typename T>
class CachingVector {
public:
    CachingVector() = delete;
    CachingVector(const Owner* const o, const std::function<int()> nf) : owner(o), nFct_(nf) {}
    CachingVector(const CachingVector&) = delete;
    T& get(int i) const {
        resize();
        return *data_.at(i);
    }
    int size() const {
        resize();
        return data_.size();
    }
    const Owner* const owner;
private:
    void resize() const {
        int n = nFct_();
        if (n==data_.size())
            return;
        data_.clear();
        for (int i=0; i<n; ++i)
            data_.push_back(std::unique_ptr<T>(std::move(new T(this, i))));
    }
    const std::function<int()> nFct_;
    mutable std::vector<std::unique_ptr<T>> data_;
};

#endif // CACHED_H
