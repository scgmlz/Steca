// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      core_io.h
//! @brief     Dataset loaders
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Antti Soininen, Jan Burle, Rebecca Brydon
//! @authors   Based on the original STeCa by Christian Randau
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

//------------------------------------------------------------------------------
}}
#endif  // CORE_IO_H
