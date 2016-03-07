/** \file
 */

#ifndef DATASET_H
#define DATASET_H

#include "panel.h"
#include "core_image.h"
#include "core_dataset.h"

namespace panel {
//------------------------------------------------------------------------------

class Dataset;

class ImageWidget: public QWidget {
  SUPER(ImageWidget,QWidget)
public:
  ImageWidget(TheHub&,Dataset&);

  void setPixmap(QPixmap const&);
  void setShowOverlay(bool);
  void setScale(uint);

  QSize sizeHint() const;

protected:
  TheHub  &theHub;
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
  DatasetOptions1(TheHub&);

private:
  QSpinBox       *spinOffsetX, *spinOffsetY;
  QDoubleSpinBox *spinDistance, *spinPixelSize;

  // TODO review
  void setTo(TheHub&);
  void setFrom(TheHub&);

  // TODO review
  void readSettings(TheHub&);
  void saveSettings();
};

class DatasetOptions2: public BoxPanel {
  SUPER(DatasetOptions2,BoxPanel) Q_OBJECT
public:
  DatasetOptions2(TheHub&);

signals:
  void imageScale(uint);

private:
  QSpinBox       *cutTop, *cutBottom, *cutLeft, *cutRight;
  QSpinBox       *spinImageScale;

  void setFrom(TheHub&);

  void readSettings();
  void saveSettings();
};

//------------------------------------------------------------------------------

class Dataset: public BoxPanel {
  SUPER(Dataset,BoxPanel)
public:
  Dataset(TheHub&);

  void setImageScale(uint);

private:
  QPixmap makePixmap(core::Image const&,core::Range rgeIntens);
  void setDataset(core::shp_Dataset);
  void renderDataset();

  core::shp_Dataset dataset;

  ImageWidget *imageWidget;
};

//------------------------------------------------------------------------------
}
#endif
