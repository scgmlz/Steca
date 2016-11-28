// ************************************************************************** //
//
// STeCa2 - StressTextureCalculator ver. 2
//
// Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.
//
// See the COPYING and AUTHORS files for more details.
// ************************************************************************** //

#include "io_io.h"

namespace io {
//------------------------------------------------------------------------------

// peek at up to maxLen bytes (to establish the file type)
static QByteArray peek(uint pos, uint maxLen, QFileInfo const& info) {
  QFile file(info.filePath());

  if (file.open(QFile::ReadOnly) && file.seek(pos))
    return file.read(maxLen);

  return QByteArray();
}

// Caress file format
static bool couldBeCaress(QFileInfo const& info) {
  static QByteArray const header("\020\012DEFCMD DAT");
  return header == peek(0, to_u(header.size()), info);
}

// Mar file format
static bool couldBeMar(QFileInfo const& info) {
  static QByteArray const header("mar research");
  return header == peek(0x80, to_u(header.size()), info);
}

// Text .dat file with metadata for tiff files
static bool couldBeTiffDat(QFileInfo const& info) {
  QFile file(info.filePath());

  if (!file.open(QFile::ReadOnly))
    return false;

  bool couldBe = false;

  QByteArray line;

  while (!(line = file.readLine()).isEmpty()) {
    str s = line;

    int commentPos = s.indexOf(';');
    if (commentPos >= 0)
      s = s.left(commentPos);

    if ((s = s.simplified()).isEmpty())
      continue;

    auto lst = s.split(' ');
    int cnt = lst.count();
    if (cnt < 2 || cnt > 4)
      return false;

    couldBe = true;
  }

  return couldBe;
}

data::shp_File load(rcstr filePath) THROWS {
  QFileInfo info(filePath);
  RUNTIME_CHECK(info.exists(), "File " % filePath % " does not exist");

  data::shp_File file;

  if (couldBeCaress(info))
    file = io::loadCaress(filePath);
  else if (couldBeMar(info))
    file = io::loadMar(filePath);
  else if (couldBeTiffDat(info))
    file = io::loadTiffDat(filePath);
  else
    THROW("unknown file type: " % filePath);

  RUNTIME_CHECK(file->datasets().count() > 0,
                "File " % filePath % " contains no datasets");

  // ensure that all datasets have images of the same size
  typ::size2d size = file->datasets().first()->imageSize();
  for (auto& dataset : file->datasets())
    if (dataset->imageSize() != size)
      THROW("Inconsistent image size in " % filePath);

  return file;
}

//------------------------------------------------------------------------------
}
// eof
