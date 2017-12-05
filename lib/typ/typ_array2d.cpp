// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      lib/typ/typ_array2d.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "typ_array2d.h"

#include "def/def_cmp_impl.h"
#include "typ/typ_json.h"

#include "test/tests.h"

namespace typ {

int size2d::compare(rc that) const {
    RET_COMPARE_VALUE(w)
    RET_COMPARE_VALUE(h)
    return 0;
}

EQ_NE_OPERATOR(size2d)


TEST("size2d", ({
         size2d sz;
         CHECK((0 == sz.w && 0 == sz.h && sz.isEmpty() && 0 == sz.count()));

         size2d sz1(1, 2);
         CHECK((1 == sz1.w && 2 == sz1.h && !sz1.isEmpty() && 2 == sz1.count()));

         sz = sz1.transposed();
         CHECK((sz.w == sz1.h && sz.h == sz1.w));
     });)


}
