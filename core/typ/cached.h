//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/cached.h
//! @brief     Defines class CACHED
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef CACHED_H
#define CACHED_H

#include <memory>
#include <vector>

//! Cached object.
template<typename T>
class Cached {
public:
    void invalidate() { valid_ = false; }
    const T& get() {
        if (!valid_) {
            cached_.recompute();
            valid_ = true;
        }
        return cached_;
    }
private:
    T cached_;
    bool valid_ {false};
};

//! Cached object with key.
template<typename T, typename K>
class KeyedCache {
public:
    void invalidate() { cached_.release(); }
    const T& get(const K key) {
        if (!cached_ || key!=key_) {
            cached_ = std::make_unique<T>(T(key));
            key_ = key;
        }
        return *cached_;
    }
private:
    std::unique_ptr<T> cached_;
    K key_;
};

//! Element of CachingVector.
template<typename T, T(*recompute)(int, int)>
class CachedElement {
public:
    void invalidate() { cached_.release(); }
    const T& get(const int i, const int n) {
        if (!cached_ || i!=i_ || n!=n_) {
            cached_ = std::make_unique<T>(recompute(i,n));
            i_ = i;
            n_ = n;
        }
        return *cached_;
    }
private:
    std::unique_ptr<T> cached_;
    int i_, n_;
};

//! Vector of individually cached objects.
template<typename Owner, typename T>
class CachingVector {
public:
    CachingVector() = delete;
    CachingVector(Owner* o) : owner_(o) {}
    void resize(int n) {
        if (n==data_.size())
            return;
        data_.clear();
        for (int i=0; i<n; ++i)
            data_.push_back(T(owner_, i, n));
    }
    const T& get(int i, int n) {
        if (n!=data_.size())
            resize(n);
        return data_.at(i);
    }
private:
    Owner* owner_;
    std::vector<T> data_;
};

#endif // CACHED_H
