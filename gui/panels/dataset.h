#ifndef IMAGE_H
#define IMAGE_H

#include "panel.h"
#include "core_image.h"
#include "session.h"

namespace panel {

class Dataset;

class ImageWidget: public QWidget {
  SUPER(ImageWidget,QWidget)
public:
  ImageWidget(Dataset&);


  void setPixmap(QPixmap const&);
  void setUpDown(bool);
  void setLeftRight(bool);
  void setTurn(int degrees);

  void setShowOverlay(bool);

  QSize sizeHint() const;

protected:
  Dataset &image;

  void resizeEvent(QResizeEvent*);
  mutable int lastHeight; // keep square

  void paintEvent(QPaintEvent*);

  QPixmap original, scaled;
  QPointF scale;

  bool upDown, leftRight; int turnDegrees; bool showOverlay;
  QSize lastPaintSize;
  QTransform transform;

public:
  void update();
};

class Dataset: public BoxPanel {
  SUPER(Dataset,BoxPanel)
public:
  Dataset(MainWin&);

private:
  void setDataset(pcCoreDataset);
  pcCoreDataset dataset;
  bool globalNorm;

public:
  Session::imagecut_t const& getCut() const;
private:
  ImageWidget *imageWidget;
  QSpinBox *cutTop, *cutBottom, *cutLeft, *cutRight;
};

}

#endif
