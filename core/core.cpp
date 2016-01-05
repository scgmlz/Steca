#include "core.h"
#include <memory>

namespace core {

void raiseError(rcstr msg) throw (Exception) {
  throw Exception(msg);
}

Session::Session() {
}

Session::~Session() {
  for (auto file: dataFiles) delete file;
}

Session::File::File(): File("") {
}

Session::File::File(rcstr fileName): info(fileName) {
}

void Session::File::load() throw (Exception) {
  if (!info.exists())
    raiseError("File " % info.filePath() % " does not exist");
}

void Session::addFile(rcstr fileName) throw(Exception) {
  if (hasFile(fileName)) return;

  std::unique_ptr<File> file(new File(fileName));
  file->load();

  dataFiles.append(file.release());
}

bool Session::hasFile(rcstr fileName) {
  QFileInfo fileInfo(fileName);
  for (auto& file: dataFiles)
    if (fileInfo == file->info) return true;
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
