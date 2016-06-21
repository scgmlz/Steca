// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      core_io.h
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "core_io.h"

namespace core { namespace io {
//------------------------------------------------------------------------------

/// peek at up to maxLen bytes (to establish the file type)
static QByteArray peek(uint maxLen, QFileInfo const& info) {
  QFile file(info.filePath());
  file.open(QFile::ReadOnly);
  return file.read(maxLen); // on error returns an empty QByteArray; that's good
}

shp_File load(rcstr filePath) THROWS {
  QFileInfo info(filePath);
  RUNTIME_CHECK(info.exists(), "File " % filePath % " does not exist");

  shp_File file;

  // apparently all Caress files begin so
  static QByteArray const caressHead("\020\012DEFCMD DAT");
  if (caressHead == peek(caressHead.size(), info)) {
    // looks like Caress, so try to load
    file = io::loadCaress(filePath);
  } else {
    THROW("unknown file type: " % filePath);
  }

  RUNTIME_CHECK(file->datasets().count() > 0,
                "File " % filePath % " contains no datasets");

  // ensure that all datasets have images of the same size
  size2d size = file->datasets().first()->imageSize();
  for (auto& dataset : file->datasets())
    if (dataset->imageSize() != size)
      THROW("Inconsistent image size in " % filePath);

  return file;
}

//------------------------------------------------------------------------------
}}
// eof
