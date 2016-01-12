#include "core_file.h"
#include "core_loadcaress.h"

namespace core {

File::File(): File(str::null) {
}

File::File(rcstr fileName): info(fileName) {
}

File::~File() {
}

void File::load() THROWS {
  RUNTIME_CHECK(info.exists(), "File " % info.filePath() % " does not exist");
  loadCaress(info.filePath(),datasets);
}

QSize File::getImageSize() const {
  QSize size;
  for (auto dataset: datasets) {
    auto imageSize = dataset->getImage().getSize();
    if (size.isEmpty())
      size = imageSize;
    else if (size != imageSize)
      return QSize();
  }
  return size;
}

Files::Files() {
}

}

// eof
