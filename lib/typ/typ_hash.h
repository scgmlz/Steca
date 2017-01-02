/*******************************************************************************
 * STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#ifndef TYP_HASH_H
#define TYP_HASH_H

#include "def/def_macros.h"
#include "def/def_gsl.h"
#include <QHash>

namespace typ {
//------------------------------------------------------------------------------

template <typename Key, typename T>
class hash : protected QHash<Key,T> {
  CLASS(hash) SUPER(QHash<Key COMMA T>)
public:
  using super::clear;
  using super::insert;
  using super::remove;
  using super::find;
  using super::contains;
  using super::value;
  using super::take;
};

template <typename Key, typename Tp>
class owning_hash : protected hash<Key,Tp> {
  CLASS(owning_hash) SUPER(hash<Key COMMA Tp>)
public:
 ~owning_hash() {
    clear();
  }

  void clear() {
    for (auto* v : QHash<Key,Tp>::values())
      delete v;
    super::clear();
  }

  void insert(Key const& key, owner_not_null<Tp> t) {
    remove(key);
    super::insert(key, t);
  }

  bool remove(Key const& key) {
    Tp t = take(key);
    delete t;
    return nullptr != t;
  }

  Tp value(Key const& key) {
    return super::value(key);
  }

  using super::contains;

  owner<Tp> take(Key const& key) {
    return owner<Tp>(super::take(key));
  }
};

//------------------------------------------------------------------------------
}
#endif // TYP_MAP_H
