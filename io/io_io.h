// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      io/io_io.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef IO_IO_H
#define IO_IO_H

#include "data/data_file.h"

namespace io {
//------------------------------------------------------------------------------

bool couldBeCaress(QFileInfo const&);
bool couldBeMar(QFileInfo const&);
bool couldBeTiffDat(QFileInfo const&);

// load a file; file type will be sensed
data::shp_File load(rcstr filePath) THROWS;

// load a Caress file
data::shp_File loadCaress(rcstr filePath) THROWS;
data::shp_File loadCaress2(rcstr filePath) THROWS;
str loadCaressComment(rcstr filePath);

// load a Mar file
data::shp_File loadMar(rcstr filePath) THROWS;

// load a metadata file + tiffs
data::shp_File loadTiffDat(rcstr filePath) THROWS;

//------------------------------------------------------------------------------
}
#endif // IO_IO_H
