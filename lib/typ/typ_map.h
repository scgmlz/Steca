// ************************************************************************** //
//
// STeCa2 - StressTextureCalculator ver. 2
//
// Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.
//
// See the COPYING and AUTHORS files for more details.
// ************************************************************************** //

#ifndef TYP_MAP_H
#define TYP_MAP_H

#include "def/def_macros.h"
#include <QMap>

namespace typ {
//------------------------------------------------------------------------------

template <typename Key, typename T>
class map : public QMap<Key,T> {
  using super_type = QMap<Key,T>;
  CLS(map) SUPER(super_type)
public:
  using super::clear;
  using super::insert;
  using super::insertMulti;
  using super::remove;
  using super::find;
  using super::contains;
  using super::value;
  using super::take;
  using super::begin;
  using super::end;
  using super::cbegin;
  using super::cend;
};

template <typename Key, typename T>
class owning_map : public map<Key,T*> {
public:
 ~owning_map() {
    for (auto* v: QMap<Key,T*>::values())
      delete v;
  }
};

//------------------------------------------------------------------------------
}
#endif // TYP_MAP_H
