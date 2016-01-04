#ifndef PANEL_IMAGES_H
#define PANEL_IMAGES_H

#include "panel.h"
#include <QOpenGLWidget>  // TODO not GL

class ImageWidget: public QOpenGLWidget {
  SUPER(ImageWidget,QOpenGLWidget)
public:
  ImageWidget();

  QSize sizeHint() const;

protected:
  void resizeEvent(QResizeEvent*);
  mutable int lastHeight; // keep square
};

class PanelImages: public Panel {
  SUPER(PanelImages,Panel)
public:
  PanelImages(MainWin&);

private:
  ImageWidget *w;
};

#endif
