#include "core_file.h"
#include "core_loadcaress.h"

namespace core {

File::File(): File(NULL_STR) {
}

File::File(rcstr fileName): info(fileName), maxIntensity(0) {
}

File::~File() {
}

void File::load() THROWS {
  RUNTIME_CHECK(info.exists(), "File " % info.filePath() % " does not exist");
  loadCaress(info.filePath(),*this,datasets);
  for (auto &dataset: datasets)
    maxIntensity = qMax(maxIntensity, dataset->getImage().maximumIntensity());
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
