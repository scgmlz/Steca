// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/io/io_io.h
//! @brief     Defines loader functions in namespace io.
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef IO_IO_H
#define IO_IO_H

#include "core/data/datafile.h"
#include "core/def/macros.h"
#include "core/typ/str.h"

class Datafile;
class QFileInfo;

namespace io {

//! load a file; file type will be sensed
shp_Datafile loadDatafile(rcstr filePath) THROWS;

str loadComment(QFileInfo const& info);

} // namespace io

#endif // IO_IO_H
