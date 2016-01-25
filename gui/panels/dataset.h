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

  void setShowOverlay(bool);

  QSize sizeHint() const;

protected:
  Dataset &dataset;

  void resizeEvent(QResizeEvent*);
  mutable int lastHeight; // keep square

  void paintEvent(QPaintEvent*);

  QPixmap original, scaled;
  QPointF scale;

  bool showOverlay;
  QSize lastPaintSize;

public:
  void update();
};

class Dataset: public BoxPanel {
  SUPER(Dataset,BoxPanel)
public:
  Dataset(MainWin&,Session&);

private:
  QPixmap makePixmap(core::Image const&,core::Image::intensity_t maxIntensity);
  void setDataset(pcCoreDataset);
  void refresh() { setDataset(dataset); }

  pcCoreDataset dataset;
  bool globalNorm;

  ImageWidget *imageWidget;
  QSpinBox *cutTop, *cutBottom, *cutLeft, *cutRight;
};

}

#endif
