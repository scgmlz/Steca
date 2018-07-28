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

#include "qcr/base/debug.h" // TMP
#include <functional>
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
template<typename Owner, typename E, E(*recompute)(const Owner* const)>
class CachedElement {
public:
    CachedElement() = delete;
    CachedElement(const Owner* const o) : owner_(o) {}
    CachedElement(const CachedElement&) = delete;
    void invalidate() { qDebug() << "INVALIDATE!"<<&*cached_; cached_.reset(nullptr); qDebug() << "INVALIDATED"; }
    E& get() {
        qDebug() << "GET1";
        if (!cached_)
            cached_ = std::make_unique<E>(recompute(owner_));
        qDebug() << "GET2        -> "<<&*cached_;
        return *cached_;
    }
    void swapPayload(CachedElement& rhs) { cached_.swap(rhs.cached_); }
private:
    const Owner* const owner_;
    std::unique_ptr<E> cached_;
};

//! Vector of individually cached objects.
template<typename Owner, typename T>
class CachingVector {
public:
    CachingVector() = delete;
    CachingVector(const Owner* const o) : owner_(o) {}
    CachingVector(const CachingVector&) = delete;
    void resize(int n) {
        if (n==data_.size())
            return;
        data_.clear();
        for (int i=0; i<n; ++i) {
            qDebug() << "RESIZE" << i;
            data_.push_back(std::make_unique<T>(T(owner_, i, n)));
        }
    }
    T& get(int i, int n) {
        if (n!=data_.size())
            resize(n);
        return *data_.at(i);
    }
private:
    const Owner* const owner_;
    std::vector<std::unique_ptr<T>> data_;
};

#endif // CACHED_H
