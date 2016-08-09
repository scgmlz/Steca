// ************************************************************************** //
//
//  Steca2:    StressTextureCalculator ver. 2
//
//! @file      io_tiff.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "io_io.h"

namespace io {
//------------------------------------------------------------------------------
/*
shp_File loadTiffs(rcstr filePath) THROWS {
  shp_File file(new File(filePath));

  QFileInfo info(filePath);

  QFile diskFile(filePath);
  RUNTIME_CHECK(diskFile.open(QIODevice::ReadOnly), "cannot open file");

  QTextStream in(&diskFile);

  vec<Dataset> datasets;
  while (!in.atEnd()) {
    str line = in.readLine();

    str_lst dataFromFile;
    for_i (line.size()) { dataFromFile.append(line.section(',', i, i)); }
  }

  return shp_File(new File(filePath));
}

void saveTiffs(rcFile file, rcstr fileName) THROWS {
  QFileInfo info(fileName);
  QDir().mkpath(info.absoluteDir().absolutePath());

  QFile diskFile(info.filePath() + ".csv");
  RUNTIME_CHECK(diskFile.open(QIODevice::WriteOnly), "cannot open file");

  QTextStream out(&diskFile);
  // generate .csv File for referencing to tiff image files
  for_i (file.numDatasets()) {
    Dataset dataset = *file.getDataset(i);
    str     fileName =
        info.absoluteFilePath() + str(".%1.tiff").arg(i, 3, 10, QChar('0'));

    QImage tiffImage(dataset.parentFile().getImageSize(), QImage::Format_RGB32);
    Image  image = dataset.getImage();
    auto   size  = image.size();

    for_i (size.height()) {
      auto y = i;
      for_i (size.width()) {
        auto x      = i;
        auto intens = image.intensity(x + y * size.width());
        tiffImage.setPixel(x, y, qRgb(0, intens, 0));
      }
    }

    tiffImage.save(fileName, "tiff");

    out << fileName;
    for_i (Dataset::numAttributes()) {
      out << ", " << dataset.getAttributeStrValue(i);
    }
    out << "\n";
    out.flush();
  }
}
*/
//------------------------------------------------------------------------------
}
// eof
