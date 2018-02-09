// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/cache.h
//! @brief     Defines and implements the templated classes cache_base, cache_lazy
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef CACHE_H
#define CACHE_H

#include "core/def/idiomatic_for.h"
#include <QSharedPointer> // no auto rm

/* Example:

struct CacheKey {
  CacheKey(int key) : key_(key) {
  }

  friend bool operator< (rc k1, rc k2) {
    return k1.key_ < k2.key_;
  }

  int key_;
};

struct CacheT {
};

void test() {
  cache_lazy<CacheKey, CacheT> cache(100);

  for_i (0xfffffu) {
    cache.insert(CacheKey(i), QSharedPointer<CacheT>(new CacheT));
    cache.value(CacheKey(0)); // keeps mru
    if (!(i & 0xffffu))
      WT(i)
  }

  cache.trim(1);
  WT(cache.value(CacheKey(0)))
}

*/

template <typename Key, typename T>
class cache_base {
public:
    typedef QSharedPointer<T> shp;

protected:
    typedef qint32 mru_t; // the higher the value, the more mru it was

    struct shp_mru_t {
        shp_mru_t() : p(), mru(0) {}
        shp_mru_t(shp p_, mru_t mru_) : p(p_), mru(mru_) {}
        shp p;
        mru_t mru; //!< most recently used
    };

    typedef QMap<Key, shp_mru_t> mapKey_t;
    typedef typename mapKey_t::iterator mapKey_it;

    mapKey_t mapKey_;

    int maxItems_;

public:
    cache_base(int maxItems) : maxItems_(maxItems) { ASSERT(maxItems_ > 0); }

    virtual ~cache_base() {}

    int count() const { return mapKey_.count(); }

    bool isEmpty() const { return mapKey_.isEmpty(); }

    virtual void trim(int n) = 0;

    void clear() { trim(0); }

    virtual shp insert(Key const&, shp p) = 0;
    virtual shp take(Key const&) = 0;
    virtual shp value(Key const&) = 0;
};

// if full, takes a hit, trims a lot
// has no overhead for each access (value())
template <typename Key, typename T>
class cache_lazy : public cache_base<Key, T> {
    using super = cache_base<Key,T>;
public:
    typedef QSharedPointer<T> shp;

private:
    using mru_t = typename super::mru_t;
    using shp_mru_t = typename super::shp_mru_t;
    using mapKey_it = typename super::mapKey_it;

    mru_t nextMru_ = 0;
    bool rollOver_ = false; // L.v.

    typedef QMap<mru_t, mapKey_it> mapMruIt_t;

    void _trim(int n) {
        if (super::count() > n) {
            mapMruIt_t mit;
            for (auto it = super::mapKey_.begin(), itEnd = super::mapKey_.end(); it != itEnd; ++it)
                mit.insert(it->mru, it);
            // make sure there were no duplicate mrus
            ASSERT(mit.count() == super::count());
            int cnt = super::count() - n;
            for (auto it = mit.begin(); cnt-- > 0; ++it)
                super::mapKey_.erase(*it);
        }

        if (super::isEmpty()) { // cleared
            nextMru_ = 0;
            rollOver_ = false;
        }
    }

    mru_t nextMru() {
        mru_t mru = nextMru_++;
        if (0 == nextMru_) // was overflow
            rollOver_ = true; // at next insert or value
        return mru;
    }

public:
    using super::super;

    void trim(int n) { _trim(n); }

    shp insert(Key const& key, shp p) {
        ASSERT(!super::mapKey_.contains(key));
        if (rollOver_)
            trim(0);
        else if (super::count() >= super::maxItems_)
            trim(super::maxItems_ / 2);
        mru_t mru = nextMru();
        super::mapKey_.insert(key, shp_mru_t(p, mru));
        return p;
    }

    shp take(Key const& key) { return super::mapKey_.take(key).p; }

    shp value(Key const& key) {
        auto it = super::mapKey_.find(key);
        if (super::mapKey_.end() == it)
            return shp();
        if (rollOver_)
            return insert(key, take(key));
        if ((it->mru + 1) != nextMru_) // not mru, update
            it->mru = nextMru();
        return it->p;
    }
};

#endif // CACHE_H
