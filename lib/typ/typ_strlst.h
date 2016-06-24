// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      typ_strlst.h
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

#ifndef TYP_STRLST_H
#define TYP_STRLST_H

#include "def/def_macros.h"
#include "def/def_gsl.h"
#include <QStringList>
#include <initializer_list>

//------------------------------------------------------------------------------

class str_lst : protected QStringList {
  WITH_SUPER(str_lst, QStringList)
public:
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

  void append(rc that) {
    super::append(that);
  }

  QString const& at(uint i) const {
    return super::at(to_i(i));
  }
};

//------------------------------------------------------------------------------
#endif // TYP_STRLST_H
