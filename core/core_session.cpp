#include "core_session.h"

namespace core {

Session::Session() {
}

Session::~Session() {
}

void Session::addFile(rcstr fileName) THROWS {
  if (hasFile(fileName)) return;

  QSharedPointer<File> file(new File(fileName));
  file->load();

  setImageSize(file->getImageSize());

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
  setImageSize();
}

bool Session::hasCorrFile() const {
  return !corrFile.isNull();
}

void Session::setCorrFile(rcstr fileName) {
  if (!fileName.isEmpty()) {
    QSharedPointer<File> file(new File(fileName));
    file->load();
    corrFile = file;
    setImageSize(file->getImageSize());
  } else {
    corrFile.clear();
    setImageSize();
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

void Session::setImageSize() {
  if (dataFiles.isEmpty() && !hasCorrFile())
    imageSize = QSize();
}

}

// eof
