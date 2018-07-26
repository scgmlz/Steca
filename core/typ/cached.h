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
    CachedElement(const Owner* const o) : owner_(o) { info("CE::CE"); }
    void invalidate() { cached_.release(); }
    void info(const char* where) {
        qDebug() << where << "i,n=" << owner_->i_ << owner_->n_ << ", owner=" << owner_ <<", this=" << this <<", this->owner=" << this->owner_; }
    E& get() {
        info("CE::get");
        if (!cached_) {
            qDebug() << "CE recompute";
            cached_ = std::make_unique<E>(recompute(owner_));
        }
        return *cached_;
    }
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
    void resize(int n) {
        qDebug() << "CachingVector resize " << n;
        if (n==data_.size())
            return;
        data_.clear();
        for (int i=0; i<n; ++i)
            data_.push_back(T(owner_, i, n));
        for (int i=0; i<n; ++i) {
            qDebug() << "CachingVector resize result[" <<i<<"]" << ": i,n="
                     << data_.at(i).i_
                     << data_.at(i).n_;
            data_.at(i).curve.info("DEB/resize");
        }
    }
    T& get(int i, int n) {
        qDebug() << "CachingVector::get/0 " << i << " of " << n;
        if (n!=data_.size())
            resize(n);
        qDebug() << "CachingVector::get/1 " << i << " of " << n;
        data_.at(i).curve.info("DEB/get");
        return data_.at(i);
    }
private:
    const Owner* const owner_;
    std::vector<T> data_;
};

#endif // CACHED_H
