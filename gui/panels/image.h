#ifndef IMAGE_H
#define IMAGE_H

#include "panel.h"
#include <QOpenGLWidget>  // TODO not GL

namespace panel {

class ImageWidget: public QOpenGLWidget {
  SUPER(ImageWidget,QOpenGLWidget)
public:
  ImageWidget();

  QSize sizeHint() const;

protected:
  void resizeEvent(QResizeEvent*);
  mutable int lastHeight; // keep square
};

class Image: public Panel {
  SUPER(Image,Panel)
public:
  Image(MainWin&);

private:
  ImageWidget *w;
};

}

#endif
