#ifndef IMAGE_H
#define IMAGE_H

#include "panel.h"
#include "core_image.h"
#include "session.h"

namespace panel {

class Image;

class ImageWidget: public QWidget {
  SUPER(ImageWidget,QWidget)
public:
  ImageWidget(Image&);

  QSize sizeHint() const;

  void setPixmap(QPixmap const&);

protected:
  Image &image;

  void resizeEvent(QResizeEvent*);
  mutable int lastHeight; // keep square

  void paintEvent(QPaintEvent*);

  QPixmap original;
  mutable QPixmap scaled;
  mutable QPointF scale;
};

class Image: public Panel {
  SUPER(Image,Panel)
public:
  Image(MainWin&);

  static QPixmap pixmapFromCoreImage(core::Image const&);

  Session::imagecut_t const& getCut() const;

private:
  ImageWidget *imageWidget;
  QSpinBox *cutTop, *cutBottom, *cutLeft, *cutRight;

  void setCutFromGui() const;
};

}

#endif
