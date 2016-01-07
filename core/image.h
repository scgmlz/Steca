#ifndef IMAGE_H
#define IMAGE_H

#include "coredefs.h"
#include <QSize>
#include <QVector>

namespace core {

class QSHARED_EXPORT Image {
public:
  typedef float type;

  Image(QSize const&,type const* intensities) THROWS;

private:
  QSize size;
  QVector<type> intensities;
};

}

#endif
