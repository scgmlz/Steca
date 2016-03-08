#include "core_file.h"

namespace core {
//------------------------------------------------------------------------------

File::File(rcstr fileName): info(fileName) {
}

File::~File() {
}

void File::fold() {
  while (datasets.count() > 1) {
    datasets.first()->addIntensities(*datasets.last());
    datasets.removeLast();
  }

  rgeIntens.invalidate();
}

void File::appendDataset(Dataset *dataset) {
  ASSERT(dataset)
  dataset->file = this;
  datasets.append(shp_Dataset(dataset));
}

QSize File::getImageSize() const {
  if (datasets.isEmpty()) return QSize(0,0);
  // guaranteed that all images have the same size; simply take the first one
  return datasets.first()->getImage().getSize();
}

Range const& File::getRgeIntens() const {
  if (!rgeIntens.isValid()) {
    for (auto const& dataset: datasets)
      rgeIntens.extend(dataset->getImage().getRgeIntens());
  }

  return rgeIntens;
}

//------------------------------------------------------------------------------
}
// eof
