// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      typ_ij.h
//! @brief     Integer coordinates
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef TYP_IJ_H
#define TYP_IJ_H

#include "def/def_macros.h"

namespace typ {
//------------------------------------------------------------------------------
// 2D point, integers

class JsonObj;

struct IJ {
  THIS(IJ)

  int i, j;

  IJ();  // (0,0)
  IJ(int, int);

  int compare(rc) const;

  JsonObj saveJson() const;
  void loadJson(JsonObj const&) THROWS;
};

//------------------------------------------------------------------------------
}
#endif // TYP_IJ_H
