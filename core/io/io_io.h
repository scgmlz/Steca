// ************************************************************************** //
//
//  Steca2:    StressTextureCalculator ver. 2
//
//! @file      io_io.h
//! @brief     Dataset loaders - main
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef IO_IO_H
#define IO_IO_H

#include "data/data_file.h"

namespace io {
//------------------------------------------------------------------------------

// load a file; file type will be sensed
data::shp_File load(rcstr filePath) THROWS;

// load a Caress file
data::shp_File loadCaress(rcstr filePath) THROWS;

//------------------------------------------------------------------------------
}
#endif // IO_IO_H
