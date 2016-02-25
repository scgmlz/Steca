#include "core_io.h"
#include "core_types.h"
#include "core_file.h"

#include "io/Caress/raw.h"

#include <sstream>
#include <cmath>

namespace core { namespace core {
//------------------------------------------------------------------------------
QVector<shp_Dataset> loadTiffs(File &file) THROWS {
  return QVector<shp_Dataset>();
}

void saveTiffs(File const&, QFileInfo const& path) {

}

//------------------------------------------------------------------------------
}}
// eof
