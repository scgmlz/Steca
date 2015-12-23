#include "core.h"

Core::Core() {
  addFile("a");
  addFile("b");
  addFile("c");
}

void Core::addFile(Core::rcstr fileName) {
  files.push_back(fileName);
}

int Core::numFiles() const {
  return files.size();
}

Core::str Core::fileName(uint i) {
  return files.at(i);
}

// eof
