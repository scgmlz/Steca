// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      core_coords.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "typ_ij.h"
#include "def/def_compare.h"
#include "typ/typ_json.h"

namespace typ {
//------------------------------------------------------------------------------

IJ::IJ() : IJ(0, 0) {}

IJ::IJ(int i_, int j_) : i(i_), j(j_) {}

int IJ::compare(rc that) const {
  COMPARE_VALUE(i)
  COMPARE_VALUE(j)
  return 0;
}

static str const KEY_I("i"), KEY_J("j");

JsonObj IJ::saveJson() const {
  return JsonObj().saveInt(KEY_I, i).saveInt(KEY_J, j);
}

void IJ::loadJson(JsonObj::rc obj) THROWS {
  i = obj.loadInt(KEY_I);
  j = obj.loadInt(KEY_J);
}

//------------------------------------------------------------------------------
}
// eof
