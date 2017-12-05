// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      lib/typ/typ_factory.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TYP_FACTORY_H
#define TYP_FACTORY_H

#include "def/def_exc.h"
#include "def/def_gsl.h"
#include "def/def_macros.h"
#include "typ/typ_hash.h"
#include "typ/typ_str.h"

namespace typ {

template <class ProductBase> class Factory {
public:
    struct MakerBase {
        virtual ~MakerBase() {}
        virtual owner_not_null<ProductBase*> make() = 0;
    };

    template <class Product> struct Maker : MakerBase {
        owner_not_null<ProductBase*> make() {
            return owner_not_null<ProductBase*>::from(new Product);
        }
    };

private:
    owning_hash<str, MakerBase*> makers_;

public:
    virtual ~Factory() {}

    void addMaker(rcstr key, owner_not_null<MakerBase*> maker) { makers_.insert(key, maker); }

    owner_not_null<ProductBase*> make(rcstr key) THROWS {
        MakerBase* maker = makers_.value(key);
        RUNTIME_CHECK(maker, "no maker " % key);
        return maker->make();
    }
};
}
#endif // TYP_FACTORY_H
