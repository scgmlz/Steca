// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_io.h
//! @brief     Dataset loaders
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_IO_H
#define CORE_IO_H

#include "core_file.h"
#include <QFileInfo>

namespace core { namespace io {
//------------------------------------------------------------------------------

/// load a file; file type will be sensed
shp_File load(rcstr filePath) THROWS;

/// load a Caress file
shp_File loadCaress(rcstr filePath) THROWS;

/// load a sequence of TIFFs
shp_File loadTiffs(rcstr filePath) THROWS;
/// save datasets as a sequence of TIFFs
void saveTiffs(File const&, rcstr filePath) THROWS;

//------------------------------------------------------------------------------
}}
#endif // CORE_IO_H
