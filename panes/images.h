#ifndef IMAGES_H
#define IMAGES_H

#include "pane.h"
#include <QListWidget>

class ImageList: public QListWidget {
  SUPER(ImageList,QListWidget)
public:
  ImageList();
};

class Images: public Pane {
  SUPER(Images,Pane) Q_OBJECT
public:
  Images();

signals:

public slots:

private:
  ImageList *l;
};

#endif
