// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      typ_types.h
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle, Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "typ_types.h"
//------------------------------------------------------------------------------

str_lst::rc normStrLst() {
  static str_lst strLst {
    "none", "monitor", "Δ monitor", "Δ time", "background",
  };

  return strLst;
}

//------------------------------------------------------------------------------
// eof
