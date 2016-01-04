#include "core.h"

Core::File::File(): File("") {
}

Core::File::File(Core::rcstr fileName): info(fileName) {
}

//------------------------------------------------------------------------------

Core::Core() {
}

Core::~Core() {
  for (auto& file: dataFiles)
    delete file;
}

void Core::addFile(rcstr fileName) {
  if (!QFileInfo(fileName).exists()) return;
  if (hasFile(fileName))             return;

  dataFiles.append(new File(fileName));
}

bool Core::hasFile(rcstr fileName) {
  QFileInfo fileInfo(fileName);
  for (auto& file: dataFiles)
    if (fileInfo == file->info) return true;
  return false;
}

uint Core::numFiles() const {
  return dataFiles.size();
}

Core::str Core::fileName(uint i) const {
  if (i == numFiles()) return correctionFileName();
  return dataFiles.at(i)->info.fileName();
}

void Core::removeFile(uint i) {
  if (i == numFiles())
    setCorrectionFile("");
  else
    dataFiles.remove(i);
}

bool Core::hasCorrectionFile() const {
  return !correctionFileName().isEmpty();
}

void Core::setCorrectionFile(rcstr fileName) {
  corrFile = fileName;
}

Core::str Core::correctionFileName() const {
  return corrFile.info.fileName();
}

// eof
