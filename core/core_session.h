#ifndef CORE_SESSION_H
#define CORE_SESSION_H

#include "core_defs.h"
#include "core_file.h"
#include <QSize>
#include <QPointF>

namespace core {

class Session {
public:
  Session();
 ~Session();

  uint numFiles(bool withCorr=false);

  void addFile(rcstr fileName) THROWS;
  bool hasFile(rcstr fileName);

  File const& getFile(uint i);
  void remFile(uint i);

  bool hasCorrFile()   const;
  void setCorrFile(rcstr fileName); // fileName may be empty -> unsets TODO

protected:
  Files dataFiles;
  QSharedPointer<File> corrFile;

  QSize imageSize;

  void setImageSize(QSize const&) THROWS;
  void updateImageSize();

public:
  struct imagecut_t {
    imagecut_t(int top = 0, int bottom = 0, int left = 0, int right = 0);
    int top, bottom, left, right;
  };

  imagecut_t const& getImageCut() const { return imageCut; }
  void setImageCut(bool topLeft, bool linked, imagecut_t const&);

protected:
  imagecut_t imageCut;
};

}

#endif
