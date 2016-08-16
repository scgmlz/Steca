// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      typ_map.h
//! @brief     Veneered QT class.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
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
