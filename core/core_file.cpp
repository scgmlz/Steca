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
  return file.read(maxLen); // on error returns an empty QByteArray; that's good
}

void File::load() THROWS {
  ASSERT(datasets.isEmpty())  // load only once

  RUNTIME_CHECK(info.exists(), "File " % info.filePath() % " does not exist");

  // apparently all Caress files begin this way
  static QByteArray const caressHead("\020\012DEFCMD DAT");
  if (caressHead == peek(caressHead.size())) {
    // looks like Caress, so try to load
    datasets = loadCaress(*this);
  } else {
    THROW("unknown file type");
  }

  RUNTIME_CHECK(!datasets.isEmpty(), "File " % info.filePath() % " contains no datasets");

  // ensure that all datasets have images of the same size
  QSize size = datasets.first()->getImage().getSize();

  for (auto const& dataset: datasets)
    if (dataset->getImage().getSize() != size)
      THROW("Inconsistent image size");

  rgeIntens.invalidate();
}

void File::fold() {
  while (datasets.count() > 1) {
    datasets.first()->addIntensities(*datasets.last());
    datasets.removeLast();
  }

  rgeIntens.invalidate();
}

QSize File::getImageSize() const {
  if (datasets.isEmpty()) return QSize();
  // guaranteed that all images have the same size; simply take the first one
  return datasets.first()->getImage().getSize();
}

Range const& File::getRgeIntens() const {
  if (rgeIntens.isInvalid()) {
    for (auto const& dataset: datasets)
      rgeIntens.extend(dataset->getImage().getRgeIntens());
  }

  return rgeIntens;
}

}

// eof
