/*******************************************************************************
 * REVIEW: STeCa2 - StressTextureCalculator ver. 2
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

#ifndef TYP_FACTORY_H
#define TYP_FACTORY_H

#include "def/def_exc.h"
#include "def/def_gsl.h"
#include "def/def_macros.h"
#include "typ/typ_hash.h"
#include "typ/typ_str.h"

namespace typ {
//------------------------------------------------------------------------------

template <class ProductBase>
class Factory {
public:
  struct MakerBase {
    virtual ~MakerBase() {}
    virtual owner_not_null<ProductBase*> make() = 0;
  };

  template <class Product>
  struct Maker : MakerBase {
    owner_not_null<ProductBase*> make() {
      return owner_not_null<ProductBase*>::from(new Product);
    }
  };

private:
  owning_hash<str, MakerBase*> makers_;

public:
  virtual ~Factory() {}

  void addMaker(rcstr key, owner_not_null<MakerBase*> maker) {
    makers_.insert(key, maker);
  }

  owner_not_null<ProductBase*> make(rcstr key) THROWS {
    MakerBase *maker = makers_.value(key);
    RUNTIME_CHECK(maker, "no maker " % key);
    return maker->make();
  }
};

//------------------------------------------------------------------------------
}
#endif // TYP_FACTORY_H
