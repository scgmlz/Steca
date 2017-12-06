// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/typ_ij.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "typ_ij.h"
#include "def/def_cmp_impl.h"
#include "typ/typ_json.h"
#include "wrap_doctest.h"

namespace typ {

IJ::IJ() : IJ(0, 0) {}

TEST("IJ()", ({
         IJ ij;
         CHECK_EQ(0, ij.i);
         CHECK_EQ(0, ij.j);
     });)

IJ::IJ(int i_, int j_) : i(i_), j(j_) {}

TEST("IJ(i,j)", ({
         IJ ij(2, 3);
         CHECK_EQ(2, ij.i);
         CHECK_EQ(3, ij.j);
     });)

int IJ::compare(rc that) const {
    RET_COMPARE_VALUE(i)
    RET_COMPARE_VALUE(j)
    return 0;
}

EQ_NE_OPERATOR(IJ)

TEST("IJ::compare", ({
         IJ ij(1, 2), ij1(1, 2), ij2(1, 0), ij3(2, 2);
         CHECK_EQ(0, ij.compare(ij));
         CHECK_EQ(0, ij.compare(ij1));
         CHECK_EQ(1, ij.compare(ij2));
         CHECK_EQ(-1, ij.compare(ij3));

         CHECK_EQ(ij, ij1);
         CHECK_NE(ij, ij2);
     });)

JsonObj IJ::saveJson() const {
    return JsonObj().saveInt(json_key::I, i).saveInt(json_key::J, j);
}

void IJ::loadJson(JsonObj::rc obj) THROWS {
    i = obj.loadInt(json_key::I);
    j = obj.loadInt(json_key::J);
}

TEST("IJ::json", ({
         IJ ij(-1, 2), ij1;
         ij1.loadJson(ij.saveJson());
         CHECK_EQ(ij, ij1);
     });)
}
