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

//#include "qcr/base/debug.h" // TMP
#include <functional>
#include <memory>
#include <vector>

//! Simple cached object.
template<typename T>
class Cached {
public:
    Cached() = delete;
    Cached(std::function<T(void)> f) : remake_(f) {}
    Cached(const Cached&) = delete;
    void invalidate() const { cached_.release(); }
    const T& get() const {
        if (!cached_)
            cached_.reset( new T{remake_()} );
        return *cached_;
    }
private:
    mutable std::unique_ptr<T> cached_;
    const std::function<T(void)> remake_;
};

//! Cached object with parent-dependent remake.
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
            cached_.reset( new T{remake_(parent)} );
        return *cached_;
    }
    const T* getif(const Parent* parent) const { return cached_ ? cached_.get() : nullptr; }
private:
    mutable std::unique_ptr<T> cached_;
    const std::function<T(const Parent*)> remake_;
};

//! Caching vector. Vector elements are recomputed when vector size changes.
template<typename Parent, typename T>
class KachingVector {
public:
    KachingVector() = delete;
    KachingVector(const std::function<int()> nFct, const std::function<T(const Parent*,int)> rFct)
        : nFct_(nFct), remake_(rFct) {}
    KachingVector(const KachingVector&) = delete;
    KachingVector(KachingVector&&) = default; // TODO rm after removal of CachingVector
    const T& get(const Parent* parent, int i) const {
        resize(parent);
        return data_.at(i);
    }
    int size(const Parent* parent) const {
        resize(parent);
        return data_.size();
    }
    void invalidate() const {
        data_.clear();
    }
    const std::vector<T> &data() const {
        return data_;
    }
private:
    void resize(const Parent* parent) const {
        int n = nFct_();
        if (n==data_.size())
            return;
        data_.clear();
        for (int i=0; i<n; ++i)
            data_.push_back(remake_(parent,i));
    }
    const std::function<int()> nFct_;
    const std::function<T(const Parent*,int)> remake_;
    mutable std::vector<T> data_;
};

//! Caching vector of cached objects.
template<typename Parent, typename T>
class SelfKachingVector : public KachingVector<Parent, Kached<Parent,T>> {
    using Base = KachingVector<Parent, Kached<Parent,T>>;
public:
    SelfKachingVector() = delete;
    SelfKachingVector(const std::function<int()> nFct,
                      const std::function<T(const Parent*,int)> rFct)
        : Base(nFct, [rFct](const Parent* p, int i){
                return Kached<Parent,T>([rFct,i](const Parent* p)->T{return rFct(p,i);}); } )
    {}
    const T& getget(const Parent* parent, int i) const { return Base::get(parent,i).get(parent); }
    void forAllValids(const Parent* parent, std::function<void(const T& t)> f) const {
        for (int i=0; i<Base::size(parent); ++i)
            if (const T* d = Base::get(parent,i).getif(parent))
                f(*d);
    }
};

//! Cached object with key.
template<typename T, typename K>
class KeyedCache {
public:
    void invalidate() const { cached_.release(); }
    const T& get(const K key) const {
        if (!cached_ || key!=key_) {
            cached_.reset( new T(key) );
            key_ = key;
        }
        return *cached_;
    }
private:
    mutable std::unique_ptr<T> cached_;
    mutable K key_;
};

#endif // CACHED_H
