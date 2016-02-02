#include "core_file.h"
#include "loaders/core_loaders.h"

namespace core {

File::File(): File(EMPTY_STR) {
}

File::File(rcstr fileName): info(fileName) {
}

File::~File() {
}

QByteArray File::peek(uint maxLen) {
  QFile file(info.filePath());
  file.open(QFile::ReadOnly);
  return file.read(maxLen); // returns an empty QByteArray if there is an error
}

void File::load() THROWS {
  RUNTIME_CHECK(info.exists(), "File " % info.filePath() % " does not exist");

  static QByteArray const caressHead("\020\012DEFCMD DAT");
  if (caressHead == peek(caressHead.size())) {
    // looks like Caress, so try to load
    datasets = loadCaress(info.filePath());
  } else {
    THROW("unknown file type");
  }

  RUNTIME_CHECK(!datasets.isEmpty(), "File " % info.filePath() % " has no datasets");

  uint size = datasets.first()->getImage().getSize();

  for (auto const& dataset: datasets)
    if (dataset->getImage().getSize() != size)
      THROW("Inconsistent image size");
}

void File::fold() {
  while (datasets.count() > 1) {
    datasets[0]->image.addIntensities(datasets.last()->image.getIntensities());
    datasets.removeLast();
  }
}

uint File::getImageSize() const {
  if (datasets.isEmpty()) return 0;
  // guaranteed that all images have the same size
  return datasets.first()->getImage().getSize();
}

}

// eof
