/** \file
 * Dataset loaders.
 */

#ifndef CORE_IO_H
#define CORE_IO_H

#include <core_file.h>
#include <QFileInfo>

namespace core { namespace io {
//------------------------------------------------------------------------------

shp_File load(rcstr filePath) THROWS;

shp_File loadCaress(rcstr filePath) THROWS;
// void saveCaress(File&) THROWS;

shp_File loadTiffs(rcstr filePath) THROWS;
void saveTiffs(File const&, QFileInfo const& path);

//------------------------------------------------------------------------------
}}
#endif
