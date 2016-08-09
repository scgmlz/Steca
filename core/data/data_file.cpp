// ************************************************************************** //
//
//  Steca2:    StressTextureCalculator ver. 2
//
//! @file      data_file.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "data_file.h"

namespace data {
//------------------------------------------------------------------------------

File::File(rcstr fileName) : fileInfo_(fileName) {
}

QFileInfo const& File::fileInfo() const {
  return fileInfo_;
}

str File::fileName() const {
  return fileInfo_.fileName();
}

//------------------------------------------------------------------------------
}
// eof
