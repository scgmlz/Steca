#ifndef CORE_IMAGE_H
#define CORE_IMAGE_H

#include "coredefs.h"
#include <QSize>
#include <QVector>

namespace core {

class QSHARED_EXPORT Image {
public:
  Image(QSize const&,int const* intensities) THROWS;

private:
  QSize size;
  QVector<int> intensities;
};

}

#endif
