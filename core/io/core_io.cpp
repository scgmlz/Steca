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
  RUNTIME_CHECK(info.exists(), "File " % info.filePath() % " does not exist");

  shp_File file;

  // apparently all Caress files begin this way
  static QByteArray const caressHead("\020\012DEFCMD DAT");
  if (caressHead == peek(caressHead.size(),info)) {
    // looks like Caress, so try to load
    file = io::loadCaress(filePath);
  } else {
    THROW("unknown file type");
  }

  RUNTIME_CHECK(file->numDatasets() > 0, "File " % info.filePath() % " contains no datasets");

  // ensure that all datasets have images of the same size
  QSize size = file->getDataset(0)->getImage().getSize();

  for_i (file->numDatasets())
    if (file->getDataset(i)->getImage().getSize() != size)
      THROW("Inconsistent image size");

  return file;
}

//------------------------------------------------------------------------------
}}
// eof
