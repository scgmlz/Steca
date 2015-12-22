#ifndef IMAGEINFO_H
#define IMAGEINFO_H

#include "pane.h"
#include <QTreeWidget>

class ImageInfoList: public QTreeWidget {
  SUPER(ImageInfoList,QTreeWidget)
public:
  ImageInfoList();

protected:
  void resizeEvent(QResizeEvent*);
};

class ImageInfoItem: public QTreeWidgetItem {
  SUPER(ImageInfoItem,QTreeWidgetItem)
public:
  ImageInfoItem(rcstr);
};

class ImageInfo: public Pane {
  SUPER(ImageInfo,Pane)
public:
  ImageInfo();

signals:

public slots:

private:
  ImageInfoList *l;
};

#endif
