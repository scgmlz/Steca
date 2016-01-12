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
  void setTurnRight(bool);
  void setTurnLeft(bool);

protected:
  Image &image;

  void resizeEvent(QResizeEvent*);
  mutable int lastHeight; // keep square

  void paintEvent(QPaintEvent*);

  QPixmap original, scaled;
  QPointF scale;

  bool upDown, leftRight, turnRight, turnLeft;
  bool retransform;
  QTransform transform;

public:
  void update();
};

class Image: public Panel {
  SUPER(Image,Panel)
public:
  Image(MainWin&);

  static QPixmap pixmapFromCoreImage(core::Image const&, int maximumIntensity);

  Session::imagecut_t const& getCut() const;

private:
  ImageWidget *imageWidget;
  QSpinBox *cutTop, *cutBottom, *cutLeft, *cutRight;

  void setCutFromGui() const;
  void setGuiFromCut() const;
};

}

#endif
