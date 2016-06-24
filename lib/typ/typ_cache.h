// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      typ_cache.h
//! @brief     A map of shared pointers limited in size, lru.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef TYP_CACHE_H
#define TYP_CACHE_H

#include "def/def_debug.h"
#include "typ/typ_map.h"
#include <QSharedPointer>
#include <QDateTime>

/* Example:

struct CacheKey {
  THIS(CacheKey)

  CacheKey(uint key) : key_(key) {
  }

  friend bool operator< (rc k1, rc k2) {
    return k1.key_ < k2.key_;
  }

  uint key_;
};

struct CacheT {
};

void test() {
  typ::cache_lru<CacheKey, CacheT> cache(100);

  for_i (13000u) {
    cache.insert(CacheKey(i), QSharedPointer<CacheT>(new CacheT));
    cache.value(CacheKey(0)); // keeps LRU
  }

  cache.trim(1);
  WT(cache.value(CacheKey(0)))
}

*/

namespace typ {
//------------------------------------------------------------------------------

template <typename Key, typename T>
class cache_lru final {
  THIS(cache_lru)
public:
  typedef QSharedPointer<T> shp;

  cache_lru(uint maxItems)
  : maxItems_(maxItems)
  , startMsec_(QDateTime::currentMSecsSinceEpoch()) {
    EXPECT(maxItems_ > 0)
  }

  uint count() const {
    return mapKey_.count();
  }

  bool isEmpty() const {
    return mapKey_.isEmpty();
  }

  void trim(uint n) {
    while (count() > n)
      removeLru();
  }

  void clear() {
    trim(0);
  }

  shp insert(Key const& key, shp p) {
    trim(maxItems_ - 1);
    shp_time shpTime(p, timNow());
    EXPECT(!mapKey_.contains(key))
    auto it = mapKey_.insert(key, shpTime);
    EXPECT(!mapTim_.contains(shpTime.tim))
    mapTim_.insert(shpTime.tim, it);
    ENSURE(mapKey_.count() == mapTim_.count())
    return p;
  }

  shp remove(Key const& key) {
    auto it1 = mapKey_.find(key);
    if (mapKey_.end() == it1)
      return shp();

    shp p = it1->p;

    auto it2 = mapTim_.find(it1->tim);
    ENSURE(mapTim_.end() != it2) // should have been there

    mapKey_.erase(it1);
    mapTim_.erase(it2);

    return p;
  }

  shp value(Key const& key) {
    shp p = remove(key);
    if (p)
      insert(key, p);
    return p;
  }

private:
  struct tim_t {
    uint msec; uint cnt;
    friend bool operator <(tim_t const& t1, tim_t const& t2) {
      if (t1.msec < t2.msec)
        return true;
      if (t1.msec > t2.msec)
        return false;
      return t1.cnt < t2.cnt;
    }
  };

  struct shp_time {
    shp_time(shp p_, tim_t tim_) : p(p_), tim(tim_) {}
    shp p; tim_t tim;
  };

  tim_t timNow() const {
    tim_t tim;
    tim.msec = uint(QDateTime::currentMSecsSinceEpoch() - startMsec_);
    tim.cnt  = 0;
    if (!mapTim_.isEmpty()) {
      tim_t max = maxTim();
      EXPECT(tim.msec >= max.msec)
      if (tim.msec == max.msec)
        tim.cnt = max.cnt + 1;  // 4G max in 1 msec
    }
    return tim;
  }

  tim_t minTim() const {
    EXPECT(!isEmpty())
    return mapTim_.firstKey();
  }

  tim_t maxTim() const {
    EXPECT(!isEmpty())
    return mapTim_.lastKey();
  }

  void removeLru() {
    EXPECT(!isEmpty())
    // if there are multiple items with the same time (insertMulti above),
    // then it is in fact Mru among them
    auto it = mapTim_.take(minTim());
    mapKey_.erase(it);
    ENSURE(mapKey_.count() == mapTim_.count())
  }

private:
  typedef map<Key, shp_time> mapKey_t;
  typedef map<tim_t, typename mapKey_t::iterator> mapTime_t;

  mapKey_t  mapKey_;
  mapTime_t mapTim_;

  uint maxItems_;

  qint64 startMsec_;
};

//------------------------------------------------------------------------------
}
#endif // TYP_CACHE_H
