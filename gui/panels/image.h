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
  void setUpDown(bool);
  void setLeftRight(bool);
  void setTurn(int degrees);

protected:
  Image &image;

  void resizeEvent(QResizeEvent*);
  mutable int lastHeight; // keep square

  void paintEvent(QPaintEvent*);

  QPixmap original, scaled;
  QPointF scale;

  bool upDown, leftRight; int turnDegrees;
  QSize lastPaintSize;
  QTransform transform;

public:
  void update();
};

class Image: public GridPanel {
  SUPER(Image,GridPanel)
public:
  Image(MainWin&);

  static QPixmap pixmapFromCoreImage(core::Image const&, int maximumIntensity);

  Session::imagecut_t const& getCut() const;

private:
  ImageWidget *imageWidget;
  QSpinBox *cutTop, *cutBottom, *cutLeft, *cutRight;
};

}

#endif
