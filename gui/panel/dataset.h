/** \file
 */

#ifndef IMAGE_H
#define IMAGE_H

#include "panel.h"
#include "core_image.h"
#include "core_dataset.h"

namespace panel {
//------------------------------------------------------------------------------

class Dataset;

class ImageWidget: public QWidget {
  SUPER(ImageWidget,QWidget)
public:
  ImageWidget(Dataset&);

  void setPixmap(QPixmap const&);
  void setShowOverlay(bool);
  void setScale(uint);

  QSize sizeHint() const;

protected:
  Dataset &dataset;
  bool showOverlay;
  QPixmap original, scaled;
  uint scale;

  void paintEvent(QPaintEvent*);
};

//------------------------------------------------------------------------------

class DatasetOptions1: public BoxPanel {
  SUPER(DatasetOptions1,BoxPanel) Q_OBJECT
public:
  DatasetOptions1(MainWin&,Session&);

  qreal const MIN_DISTANCE   = 1.;  // mm
  qreal const MIN_PIXEL_SIZE = .01; // mm

private:
  QSpinBox       *spinOffsetX, *spinOffsetY;
  QDoubleSpinBox *spinDistance, *spinPixelSize;

  void setTo(Session&);
  void setFrom(Session&);

  // TODO to Session() of MainWin()
  void readSettings(Session&);
  void saveSettings();
};

class DatasetOptions2: public BoxPanel {
  SUPER(DatasetOptions2,BoxPanel) Q_OBJECT
public:
  DatasetOptions2(MainWin&,Session&);

signals:
  void imageScale(uint);

private:
  QSpinBox       *cutTop, *cutBottom, *cutLeft, *cutRight;
  QSpinBox       *spinImageScale;

  void setFrom(Session&);

  void readSettings();
  void saveSettings();
};

//------------------------------------------------------------------------------

class Dataset: public BoxPanel {
  SUPER(Dataset,BoxPanel)
public:
  Dataset(MainWin&,Session&);

  void setImageScale(uint);

private:
  QPixmap makePixmap(core::Image const&,core::Range rgeIntens,core::Image* corr);
  void setDataset(core::shp_Dataset);
  void refresh();

  core::shp_Dataset dataset;
  bool globalNorm;

  ImageWidget *imageWidget;
};

//------------------------------------------------------------------------------
}
#endif
