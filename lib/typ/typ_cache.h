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
  CLS(CacheKey)

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
  typ::cache<CacheKey, CacheT> cache(100);

  for_i (13000u) {
    cache.insert(CacheKey(i), QSharedPointer<CacheT>(new CacheT));
    cache.value(CacheKey(0)); // keeps mru
  }

  cache.trim(1);
  WT(cache.value(CacheKey(0)))
}

*/

namespace typ {
//------------------------------------------------------------------------------

template <typename Key, typename T>
class cache_base {
  CLS(cache_base)
public:
  typedef QSharedPointer<T> shp;

protected:
  typedef uint mru_t; // the higher, the more mru

  struct shp_mru_t {
    shp_mru_t(shp p_, mru_t mru_) : p(p_), mru(mru_) {}
    shp p; mru_t mru;
  };

  typedef map<Key, shp_mru_t> mapKey_t;

  mapKey_t mapKey_;

  uint  maxItems_;

public:
  cache_base(uint maxItems) : maxItems_(maxItems) {
    EXPECT(maxItems_ > 0)
  }

  virtual ~cache_base() {
  }

  uint count() const {
    return mapKey_.count();
  }

  bool isEmpty() const {
    return mapKey_.isEmpty();
  }

  virtual void trim(uint n) = 0;

  void clear() {
    trim(0);
  }

  virtual shp insert(Key const&, shp p) = 0;
  virtual shp remove(Key const&)        = 0;
  virtual shp value(Key const&)         = 0;
};

//------------------------------------------------------------------------------

template <typename Key, typename T>
class cache_eager final : public cache_base<Key,T> {
  CLS(cache_eager) SUPER(cache_base<Key COMMA T>)
public:
  typedef QSharedPointer<T> shp;

private:
  using mru_t     = typename super::mru_t;
  using shp_mru_t = typename super::shp_mru_t;
  using mapKey_t  = typename super::mapKey_t;

  typedef map<mru_t, typename mapKey_t::iterator> mapMru_t;
  mapMru_t mapMru_;

public:
  using super::super;

  void trim(uint n) {
    while (super::count() > n)
      removeLru();
  }

  shp insert(Key const& key, shp p) {
    EXPECT(!mapKey_.contains(key))
    trim(super::maxItems_ - 1);

    mru_t nextMru = 0;
    if (!super::isEmpty()) {
      nextMru = mapMru_.lastKey() + 1;
      if (0 == nextMru) // was overflow
        super::clear(); // take a hit, but only every 4G operations
    }

    shp_mru_t shpMru(p, nextMru);
    EXPECT(!mapMru_.contains(shpMru.mru))
    auto it = super::mapKey_.insert(key, shpMru);
    mapMru_.insert(shpMru.mru, it);
    ENSURE(mapKey_.count() == mapMru_.count())
    return p;
  }

  shp remove(Key const& key) {
    auto it1 = super::mapKey_.find(key);
    if (super::mapKey_.end() == it1)
      return shp();

    shp p = it1->p;

    auto it2 = mapMru_.find(it1->mru);
    ENSURE(mapMru_.end() != it2)

    super::mapKey_.erase(it1);
    mapMru_.erase(it2);

    return p;
  }

  shp value(Key const& key) {
    shp p = remove(key);  // remove + insert makes it mru
    if (p)
      insert(key, p);
    return p;
  }

private:
  void removeLru() {
    EXPECT(!isEmpty())
    auto itMru = mapMru_.begin();
    auto itKey = *itMru;
    mapMru_.erase(itMru);
    super::mapKey_.erase(itKey);
    ENSURE(mapKey_.count() == mapMru_.count())
  }
};

//------------------------------------------------------------------------------
}
#endif // TYP_CACHE_H
