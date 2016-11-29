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

#ifndef TYP_STRLST_H
#define TYP_STRLST_H

#include "def/def_macros.h"
#include "def/def_gsl.h"
#include <QStringList>
#include <initializer_list>

//------------------------------------------------------------------------------

class str_lst : protected QStringList {
  CLS(str_lst) WITH_SUPER(QStringList)
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
