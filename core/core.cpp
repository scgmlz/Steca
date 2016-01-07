#include "core.h"
#include <memory>
#include "loadcaress.h"

namespace core {

Session::Session() {
}

Session::~Session() {
}

void Session::addFile(rcstr fileName) THROWS {
  if (hasFile(fileName)) return;

  std::unique_ptr<File> file(new File(fileName));
  file->load();

  dataFiles.append(file.release());
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
  corrFile = fileName;
}

}

// eof
