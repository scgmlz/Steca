/** \file
 * Dataset loaders.
 */

#ifndef CORE_LOADERS_H
#define CORE_LOADERS_H

#include <core_dataset.h>
#include <QFileInfo>

namespace core { namespace io {
//------------------------------------------------------------------------------

QVector<shp_Dataset> loadCaress(File&) THROWS;
// void saveCaress(File&) THROWS;

QVector<shp_Dataset> loadTiffs(File&) THROWS;
void saveTiffs(File const&, QFileInfo const& path);

//------------------------------------------------------------------------------
}}
#endif
