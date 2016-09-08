// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      io_caress.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "io_io.h"

namespace io {
//------------------------------------------------------------------------------

data::shp_File loadMar(rcstr filePath) THROWS {
  data::shp_File file(new data::File(filePath));
  return file;
}

//------------------------------------------------------------------------------
}
// eof
