// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/io/io_io.h
//! @brief     Defines loader functions in namespace io.
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef IO_IO_H
#define IO_IO_H

#include "data/datafile.h"

namespace io {

bool couldBeCaress(QFileInfo const&);
bool couldBeMar(QFileInfo const&);
bool couldBeTiffDat(QFileInfo const&);

//! load a file; file type will be sensed
data::shp_Datafile load(rcstr filePath) THROWS;

str loadCaressComment(rcstr filePath);

} // namespace io

#endif // IO_IO_H
