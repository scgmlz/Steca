#include "core.h"

Core::Core() {
  addFile("a"); // TODO out
  addFile("b b b b b b b b b b b b b b b b b b b b b b b b b");
  addFile("c");
  setCorrectionFile("cc");
}

void Core::addFile(rcstr fileName) {
  files.push_back(fileName);
}

uint Core::numFiles() const {
  return files.size();
}

Core::str Core::fileName(uint i) const {
  if (i == numFiles()) return correctionFile();
  return files.at(i);
}

bool Core::hasCorrectionFile() const {
  return !corrFile.empty();
}

void Core::setCorrectionFile(rcstr fileName) {
  corrFile = fileName;
}

Core::str Core::correctionFile() const {
  return corrFile;
}

// eof
