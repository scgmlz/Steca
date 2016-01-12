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

  auto size = file->getImageSize();
  if (size.isEmpty())
    THROW(fileName % " inconsistent image size");

  if (imageSize.isEmpty())
    imageSize = size;
  else if (imageSize != size)
    THROW(fileName % " session-inconsistent image size");

  dataFiles.append(file);
}

bool Session::hasFile(rcstr fileName) {
  QFileInfo fileInfo(fileName);
  for (auto& file: dataFiles)
    if (fileInfo == file->getInfo()) return true;
  return false;
}

void Session::remFile(uint i) {
  dataFiles.remove(i);
}

bool Session::hasCorrFile() const {
  return !corrFile.name().isEmpty();
}

void Session::setCorrFile(rcstr fileName) {
  corrFile = File(fileName);
  if (!fileName.isEmpty()) corrFile.load();
}

}

// eof
