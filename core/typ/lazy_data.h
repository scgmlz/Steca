//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/lazy_data.h
//! @brief     Defines and implements the class templates Cached, CachingVector, KeyedCache
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef LAZY_DATA_H
#define LAZY_DATA_H

#include <functional>
#include <memory>
#include <vector>

//! Data caches that are evaluated just in time.

//! This namespace contains the class templates Cached, CachingVector, KeyedCache.
//! These classes hold payload data and computation methods. Payload data are computed
//! when needed for the first time. Then they remain cached until explicitly invalidated.

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
    const TPayload& yield(TRemakeArgs... args) const {
        if (!cached_)
            cached_.reset( new TPayload{remake_(args...)} );
        return *cached_;
    }
    const TPayload* current() const { return cached_ ? cached_.get() : nullptr; }
private:
    mutable std::unique_ptr<TPayload> cached_;
    const std::function<TPayload(TRemakeArgs...)> remake_;
};

//! Caching vector of cached objects.
template<typename TPayload, typename... TRemakeArgs>
class VectorCache {
public:
    VectorCache() = delete;
    VectorCache(const std::function<int()> sizeFunction,
                const std::function<TPayload(int, TRemakeArgs...)> remakeOne)
        : sizeFunction_(sizeFunction)
        , remakeOne_(remakeOne)
        {}
    VectorCache(const VectorCache&) = delete;
    VectorCache(VectorCache&&) = default;
    void clear_vector() const { data_.clear(); }
    void invalidate_at(int i) const { data_.at(i).invalidate(); }
    int size(TRemakeArgs... args) const {
        check_size(args...);
        return data_.size();
    }
    const TPayload& yield_at(int i, TRemakeArgs... args) const {
        check_size(args...);
        return data_.at(i).yield(args...);
    }
    void forAllValids(std::function<void(const TPayload& t)> f) const {
        for (int i=0; i<data_.size(); ++i)
            if (const TPayload* d = data_.at(i).current())
                f(*d);
    }
    const std::vector<Cached<TPayload,TRemakeArgs...>> &vecRef() const { return data_; }
private:
    void check_size(TRemakeArgs... args) const {
        int n = sizeFunction_();
        if (n==data_.size())
            return;
        data_.clear();
        // initialize individual caches (without computing their payload)
        for (int i=0; i<n; ++i)
            data_.push_back({[this,i](TRemakeArgs... args)->TPayload{
                        return remakeOne_(i,args...);}});
    }
    mutable std::vector<Cached<TPayload,TRemakeArgs...>> data_;
    const std::function<int()> sizeFunction_;
    const std::function<TPayload(int,TRemakeArgs...)> remakeOne_;
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
