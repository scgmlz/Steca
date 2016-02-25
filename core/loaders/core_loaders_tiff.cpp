#include "core_loaders.h"
#include "core_types.h"
#include "core_file.h"

#include "loaders/Caress/raw.h"

#include <sstream>
#include <cmath>

namespace core {

QVector<shp_Dataset> loadTiff(QFile &file) THROWS {
  // Missing check if File exists and is readable

  QVector<shp_Dataset> datasets;

  try{

    QTextStream in(&file);

    while (!in.atEnd()) {
      // check if comments in first line or empty still missing
      QString line = "";
      QString pictureFileName = "";
      qreal phi = 0, monitor = 0, expTime = 0;

      for (int i = 0; i < 4; ++i) {
        line = in.readLine();

        switch(i){
          case 0:
            pictureFileName = line;
            break;
          case 1:
            phi = line.toDouble();
            break;
          case 2:

            break;
          case 3:

            break;
        }


      }


    }

  } catch (Exception e) {
    close_data_file();
    throw;

  }

  return datasets;
}













}
