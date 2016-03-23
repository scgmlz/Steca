#include "core_io.h"
#include "core_types.h"
#include "core_file.h"
#include "core_dataset.h"
#include "core_session.h"

#include <QTextStream>
#include <QImage>
#include <QImageWriter>

#include "io/Caress/raw.h" // Must be included after QImage.

#include <QDir>
#include <sstream>
#include <cmath>

namespace core { namespace io {
//------------------------------------------------------------------------------
shp_File loadTiffs(rcstr filePath) THROWS {
  shp_File file(new File(filePath));

  QFileInfo info(filePath);

  QFile diskFile(filePath);
  RUNTIME_CHECK(diskFile.open(QIODevice::ReadOnly), "cannot open file");

  QTextStream in(&diskFile);

  QVector<Dataset> datasets;
  while (!in.atEnd()) {
    str line = in.readLine();

    QVector<str> dataFromFile;
    for (int i = 0; i < line.size(); ++i) {
      dataFromFile.append(line.section(',',i,i));
    }

  }

  return shp_File(new File(filePath));
}



void saveTiffs(File const& file, rcstr fileName) THROWS {
  QFileInfo info(fileName);
  QDir().mkpath(info.absoluteDir().absolutePath());

  QFile diskFile(info.filePath() + ".csv");
  RUNTIME_CHECK(diskFile.open(QIODevice::WriteOnly),"cannot open file");

  QTextStream out(&diskFile);
  // generate .csv File for referencing to tiff image files
  for_i (file.numDatasets()) {
    Dataset dataset = *file.getDataset(i);
    str fileName = info.absoluteFilePath() + QString(".%1.tiff").arg(i,3,10,QChar('0'));

    QImage tiffImage(dataset.getFile().getImageSize(),QImage::Format_RGB32);
    Image image = dataset.getImage();
    auto size = image.size();

    for_i (size.height()) {
      auto y = i;
      for_i (size.width()) {
        auto x = i;
        auto intens = image.intensity(x + y*size.width());
        tiffImage.setPixel(x, y, qRgb(0,intens,0));
      }
    }

    tiffImage.save(fileName,"tiff");

    out << fileName;
    for_i (Dataset::NUM_ATTRIBUTES) {
      out << ", " << dataset.getAttributeStrValue(i);
    }
    out << "\n"; out.flush();
  }

}


//------------------------------------------------------------------------------
}}
// eof
