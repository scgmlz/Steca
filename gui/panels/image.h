#ifndef IMAGE_H
#define IMAGE_H

#include "panel.h"
#include "core_image.h"

namespace panel {

class ImageWidget: public QWidget {
  SUPER(ImageWidget,QWidget)
public:
  ImageWidget();

  QSize sizeHint() const;

  void setPixmap(QPixmap const&);

protected:
  void resizeEvent(QResizeEvent*);
  mutable int lastHeight; // keep square

  void paintEvent(QPaintEvent*);

  QPixmap original;
  mutable QPixmap scaled;

};

class Image: public Panel {
  SUPER(Image,Panel)
public:
  Image(MainWin&);

  static QPixmap pixmapFromCoreImage(core::Image const&);

private:
  ImageWidget *w;
};

}

#endif
