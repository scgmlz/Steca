#include "core_session.h"

namespace core {

Session::Session() {
}

Session::~Session() {
}

uint Session::numFiles(bool withCorr) {
  return dataFiles.count() + (withCorr && hasCorrFile() ? 1 : 0);
}

void Session::addFile(rcstr fileName) THROWS {
  if (hasFile(fileName)) return;

  QSharedPointer<File> file(new File(fileName));
  file->load();

  setImageSize(file->getDatasets().getImageSize());

  dataFiles.append(file);
}

bool Session::hasFile(rcstr fileName) {
  QFileInfo fileInfo(fileName);
  for (auto& file: dataFiles)
    if (fileInfo == file->getInfo()) return true;
  return false;
}

File const& Session::getFile(uint i) {
  if ((uint)dataFiles.count() == i) {
    ASSERT(!corrFile.isNull())
    return *corrFile;
  } else {
    return *dataFiles.at(i);
  }
}

void Session::remFile(uint i) {
  dataFiles.remove(i);
  updateImageSize();
}

bool Session::hasCorrFile() const {
  return !corrFile.isNull();
}

void Session::setCorrFile(rcstr fileName) {
  if (!fileName.isEmpty()) {
    QSharedPointer<File> file(new File(fileName));
    file->load();
    corrFile = file;
    setImageSize(file->getDatasets().getImageSize());
  } else {
    corrFile.clear();
    updateImageSize();
  }
}

void Session::setImageSize(QSize const& size) THROWS {
  if (size.isEmpty())
    THROW(" inconsistent image size");
  if (imageSize.isEmpty())
    imageSize = size;
  else if (imageSize != size)
    THROW(" session-inconsistent image size");
}

void Session::updateImageSize() {
  if (dataFiles.isEmpty() && !hasCorrFile())
    imageSize = QSize();
}

Session::imagecut_t::imagecut_t(int top_, int bottom_, int left_, int right_)
: top(top_), bottom(bottom_), left(left_), right(right_) {
}

void Session::setImageCut(bool topLeft, bool linked, imagecut_t const& imageCut_) {
  if (imageSize.isEmpty())
    imageCut = imagecut_t();
  else {
    auto limit = [linked](int &thisOne, int &thatOne, int maxTogether) {
      if (linked && thisOne+thatOne>=maxTogether) {
        thisOne = thatOne = qMax(0, (maxTogether-1) / 2);
      } else {
        thisOne = qMax(qMin(thisOne, maxTogether - thatOne - 1), 0);
        thatOne = qMax(qMin(thatOne, maxTogether - thisOne - 1), 0);
      }
    };

    imageCut = imageCut_;
    // make sure that cut values are valid; in the right order
    if (topLeft) {
      limit(imageCut.top,   imageCut.bottom,  imageSize.height());
      limit(imageCut.left,  imageCut.right,   imageSize.width());
    } else {
      limit(imageCut.bottom,imageCut.top,     imageSize.height());
      limit(imageCut.right, imageCut.left,    imageSize.width());
    }
  }

}

}

// eof
