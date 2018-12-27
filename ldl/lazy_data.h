//  ***********************************************************************************************
//
//  Lazy Data Library
//
//! @file      lazy_data.h
//! @brief     Defines and implements the class templates Cached, CachingVector, KeyedCache, ...
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   The Boost Licence
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke, Scientific Computing Group at MLZ <j.wuttke@fz-juelich.de>
//
//  ***********************************************************************************************

#ifndef LAZY_DATA_H
#define LAZY_DATA_H

#include <functional>
#include <memory>
#include <vector>

namespace lazy_data {

//! Simple cached object.
template<typename TPayload, typename... TRemakeArgs>
class Cached {
public:
    Cached() = delete;
    Cached(std::function<TPayload(TRemakeArgs...)> f) : remake_(f) {}
    Cached(const Cached&) = delete;
    Cached(Cached&&) = default;
    void invalidate() const { cached_.release(); }
    const TPayload& get(TRemakeArgs... args) const {
        if (!cached_)
            cached_.reset( new TPayload{remake_(args...)} );
        return *cached_;
    }
    const TPayload* getif() const { return cached_ ? cached_.get() : nullptr; }
private:
    mutable std::unique_ptr<TPayload> cached_;
    const std::function<TPayload(TRemakeArgs...)> remake_;
};

//! Caching vector. Vector elements are recomputed when vector size changes.
template<typename Parent, typename TPayload>
class KachingVector {
public:
    KachingVector() = delete;
    KachingVector(const std::function<int()> nFct,
                  const std::function<TPayload(const Parent*,int)> rFct)
        : nFct_(nFct), remake_(rFct) {}
    KachingVector(const KachingVector&) = delete;
    KachingVector(KachingVector&&) = default;
    const TPayload& get(const Parent* parent, int i) const {
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
    const std::vector<TPayload> &data() const {
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
    const std::function<TPayload(const Parent*,int)> remake_;
    mutable std::vector<TPayload> data_;
};

//! Caching vector of cached objects.
template<typename Parent, typename TPayload>
class SelfKachingVector : public KachingVector<Parent, Cached<TPayload,const Parent*>> {
    using Base = KachingVector<Parent, Cached<TPayload,const Parent*>>;
public:
    SelfKachingVector() = delete;
    SelfKachingVector(const std::function<int()> nFct,
                      const std::function<TPayload(const Parent*,int)> rFct)
        : Base(nFct, [rFct](const Parent* p, int i){
                return Cached<TPayload,const Parent*>(
                    [rFct,i](const Parent* p)->TPayload{return rFct(p,i);}); } )
    {}
    const TPayload& getget(const Parent* parent, int i) const {
        return Base::get(parent,i).get(parent); }
    void forAllValids(const Parent* parent, std::function<void(const TPayload& t)> f) const {
        for (int i=0; i<Base::size(parent); ++i)
            if (const TPayload* d = Base::get(parent,i).getif())
                f(*d);
    }
};

//! Cached object with key.
template<typename TPayload, typename TKey>
class KeyedCache {
public:
    void invalidate() const { cached_.release(); }
    const TPayload& get(const TKey key) const {
        if (!cached_ || key!=key_) {
            cached_.reset( new TPayload(key) );
            key_ = key;
        }
        return *cached_;
    }
private:
    mutable std::unique_ptr<TPayload> cached_;
    mutable TKey key_;
};

} // namespace lazy_data

#endif // LAZY_DATA_H
