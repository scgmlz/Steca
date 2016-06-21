// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      type_hash.h
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

#ifndef TYPE_VEC_H
#define TYPE_VEC_H

#include "types_inc_macros.h"
#include <QHash>
#include <initializer_list>

/* The derived classes control access to Qt classes:
 * - remove duplicities (count/size)
 * - use uint instead of int
 */

//------------------------------------------------------------------------------

template <typename Key, typename T>
class hash : protected QHash<Key,T> {
  using hash_super = QHash<Key,T>;
  SUPER(hash, hash_super)
public:
  super const& q() const { return *this; }  // access the Qt super class
};

//------------------------------------------------------------------------------
#endif  // TYPES_INC_VEC_H
