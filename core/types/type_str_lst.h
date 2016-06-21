// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      type_str_lst.h
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

#ifndef TYPE_STR_LST_H
#define TYPE_STR_LST_H

#include "types_inc_macros.h"
#include <QStringList>
#include <initializer_list>

/* The derived classes control access to Qt classes:
 * - remove duplicities (count/size)
 * - use uint instead of int
 */

//------------------------------------------------------------------------------

class str_lst : protected QStringList {
  SUPER(str_lst, QStringList)
public:
  super const& q() const { return *this; }  // access the Qt super class

  str_lst()                                 : super()     {}
  str_lst(std::initializer_list<QString> l) : super(l)    {}
  str_lst(super const& that)                : super(that) {}

  using super::count;
  using super::clear;
  using super::isEmpty;
  using super::begin;
  using super::end;
  using super::cbegin;
  using super::cend;
  using super::append;
  using super::removeLast;

  void append(str_lst const& that) { super::append(that);      }
  QString const& at(uint i) const  { return super::at(int(i)); }
};

//------------------------------------------------------------------------------
#endif  // TYPE_STR_LST_H
