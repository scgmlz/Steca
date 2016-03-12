#ifndef CORE_IMAGE_CUT_H
#define CORE_IMAGE_CUT_H

#include <QtGlobal>
#include <QSize>

namespace core {
//------------------------------------------------------------------------------

struct ImageCut {
  ImageCut(uint top = 0, uint bottom = 0, uint left = 0, uint right = 0);
  bool operator==(ImageCut const&);
  uint top, bottom, left, right;

  uint getWidth(QSize const&) const;
  uint getHeight(QSize const&) const;
  uint getCount(QSize const&) const;
};

//------------------------------------------------------------------------------
}
#endif