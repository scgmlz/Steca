#ifndef IMAGE_H
#define IMAGE_H

#include "pane.h"
#include <QOpenGLWidget>

class ImageWidget: public QOpenGLWidget {
  SUPER(ImageWidget,QOpenGLWidget)
public:
  ImageWidget();

  QSize sizeHint() const;

protected:
  void resizeEvent(QResizeEvent*);
  mutable int lastHeight; // keep square
};

class Image: public Pane {
  SUPER(Image,Pane) Q_OBJECT
public:
  Image();

signals:

public slots:

private:
  ImageWidget *w;
};

#endif
