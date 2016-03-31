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
  datasets.append(shp_Dataset(dataset));
}

QSize File::getImageSize() const {
  if (datasets.isEmpty()) return QSize(0,0);
  // guaranteed that all images have the same size; simply take the first one
  return datasets.first()->getImage().size();
}

Range const& File::intensRange() const {
  if (!rgeIntens.isValid()) {
    for (auto const& dataset: datasets)
      rgeIntens.extend(dataset->getImage().intensRange());
  }

  return rgeIntens;
}

qreal File::calAverageMonitor() const {
  qreal averMon = 0;
  for (auto const& dataset: datasets) {
    averMon += dataset->monitorCount();
  }
  averMon = averMon/datasets.count();
  return averMon;
}

qreal File::calAverageDeltaTime() const {
  qreal averDTime = 0;
  for (auto const& dataset: datasets) {
    averDTime += dataset->deltaTime();
  }
  averDTime = averDTime/datasets.count();
  return averDTime;
}

//------------------------------------------------------------------------------
}
// eof
