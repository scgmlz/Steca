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
class CachedPayload {
public:
    CachedPayload() = default;
    CachedPayload(const Owner* const o) : owner_(o) {}
    CachedPayload(const Owner* const o, CachedPayload& c) : owner_(o) { payload_.swap(c.payload_); }
    CachedPayload(const CachedPayload&) = delete;
    void invalidate() { payload_.reset(nullptr); }
    E& get() const {
        if (!owner_)
            qFatal("CachedPayload::get called before owner was set; missing Payload::init()?");
        if (!payload_)
            payload_ = std::make_unique<E>(recompute(owner_));
        return *payload_;
    }
private:
    mutable std::unique_ptr<E> payload_;
    const Owner* const owner_ {nullptr};
};


//! Vector of individually cached objects.
template<typename Owner, typename T>
class CachingVector {
public:
    CachingVector() = delete;
    CachingVector(const Owner* const o, const std::function<int()> nf) : owner_(o), nFct_(nf) {}
    CachingVector(const CachingVector&) = delete;
    T& get(int i) const {
        resize();
        return *data_.at(i);
    }
    int size() const {
        resize();
        return data_.size();
    }
private:
    void resize() const {
        int n = nFct_();
        if (n==data_.size())
            return;
        data_.clear();
        for (int i=0; i<n; ++i) {
            data_.push_back(std::make_unique<T>(T(owner_, i)));
            data_.back()->init();
        }
    }
    const Owner* const owner_;
    const std::function<int()> nFct_;
    mutable std::vector<std::unique_ptr<T>> data_;
};

#endif // CACHED_H
