#include "core_io.h"
#include "core_types.h"
#include "core_file.h"
#include "core_dataset.h"
#include "core_session.h"
#include "io/Caress/raw.h"


#include <QDir>
#include <sstream>
#include <cmath>

namespace core { namespace io {
//------------------------------------------------------------------------------
shp_File loadTiffs(rcstr filePath) THROWS {
  return shp_File(new File(filePath));
}

void saveTiffs(File const& file, rcstr fileName) THROWS {
  QFileInfo info(fileName);

  WT(info.absoluteDir().absolutePath())
  QDir().mkpath(info.absoluteDir().absolutePath());

  QFile diskFile(info.filePath() + ".csv");

  RUNTIME_CHECK(diskFile.open(QIODevice::WriteOnly),"cannot ...");

  QTextStream out(&diskFile);
  QVector<Dataset> datasets; // not from here

  for_i (file.numDatasets()) {
    Dataset dataset = *file.getDataset(i);
    str fileName = info.fileName() + QString(".%1.tiff").arg(i,3,10,QChar('0'));
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
