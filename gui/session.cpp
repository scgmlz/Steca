#include "session.h"

Session::Session(): coreSession(new core::Session) {
}

Session::~Session(){
  delete coreSession;
}

void Session::addFile(rcstr filePath) {
  coreSession->addFile(filePath);
  emit filesChanged();
}

void Session::addFiles(str_lst filePaths) {
  for (auto& filePath: filePaths)
    coreSession->addFile(filePath);
  emit filesChanged();
}

bool Session::hasFile(rcstr filePath) {
  return coreSession->hasFile(filePath);
}

void Session::remFile(uint i) {
  auto &files = coreSession->getDataFiles();
  if ((uint)files.count() == i)
    coreSession->setCorrFile(str::null);
  else
    coreSession->remFile(i);
  emit filesChanged();
}

uint Session::numFiles(bool withCorr) {
  return coreSession->getDataFiles().count()
      + (withCorr && hasCorrFile() ? 1 : 0);
}

core::File const& Session::getFile(uint i) {
  auto &files = coreSession->getDataFiles();
  return ((uint)files.count() == i)
    ? coreSession->getCorrFile()
    : files.at(i)->name();
}

str Session::fileName(uint i) {
  return getFile(i).name();
}

bool Session::hasCorrFile() {
  return coreSession->hasCorrFile();
}

void Session::setCorrFile(rcstr filePath) {
  coreSession->setCorrFile(filePath);
  emit filesChanged();
}

str Session::corrFileName() {
  return coreSession->getCorrFile().name();
}

// eof
