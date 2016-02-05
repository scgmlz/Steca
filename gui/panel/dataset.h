/** \file
 */

#ifndef IMAGE_H
#define IMAGE_H

#include "panel.h"
#include "core_image.h"
#include "core_dataset.h"

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
  void paintEvent(QPaintEvent*);

  QPixmap original, scaled;
  QPointF scale;

  bool showOverlay;
};

class DatasetOptions: public BoxPanel {
  SUPER(DatasetOptions,BoxPanel)
public:
  DatasetOptions(MainWin&,Session&);

private:
  QSpinBox  *cutTop, *cutBottom, *cutLeft, *cutRight;
  QCheckBox *checkIsBeamOffset;
  QSpinBox  *spinOffsetX, *spinOffsetY;

  void setTo(Session&);
  void setFrom(Session&);

  void readSettings(Session&);
  void saveSettings();
};

class Dataset: public BoxPanel {
  SUPER(Dataset,BoxPanel)
public:
  Dataset(MainWin&,Session&);

private:
  QPixmap makePixmap(core::Image const&,core::Range rgeIntens,core::Image* corr);
  void setDataset(core::shp_Dataset);
  void refresh();

  core::shp_Dataset dataset;
  bool globalNorm;

  ImageWidget *imageWidget;
};

}

#endif
