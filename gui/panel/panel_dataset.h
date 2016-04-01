/** \file
 */

#ifndef PANEL_DATASET_H
#define PANEL_DATASET_H

#include "panel.h"
#include "core_dataset.h"
#include "models.h"

#include <QScrollArea>

//namespace model {
//class DatasetViewModel;
//}

namespace panel {
//------------------------------------------------------------------------------

class DatasetView: public HubListView {
  SUPER(DatasetView,HubListView)
public:
  using Model = model::DatasetViewModel;

  DatasetView(TheHub&);

protected:
  void selectionChanged(QItemSelection const&, QItemSelection const&);

private:
  Model &model;
};

//------------------------------------------------------------------------------

class DockDatasets: public DockWidget {
  SUPER(DockDatasets,DockWidget)
public:
  DockDatasets(TheHub&);
private:
  DatasetView *datasetView;
};

//------------------------------------------------------------------------------

class DockDatasetInfo: public DockWidget {
  SUPER(DockDatasetInfo,DockWidget)
public:
  DockDatasetInfo(TheHub&);

private:
  class Info: public QWidget {
  public:
    Info(model::infoitem_vec&);
    QGridLayout *grid;
  };

  TheHub &theHub;
  Info *info;
  model::infoitem_vec infoItems;
};

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
  SUPER(DatasetOptions1,BoxPanel)
  Q_OBJECT
public:
  DatasetOptions1(TheHub&);

private:
  QSpinBox       *spinOffsetX, *spinOffsetY;
  QDoubleSpinBox *spinDistance, *spinPixelSize;
  QComboBox      *comboNormType;

  // REVIEW
  void setTo(TheHub&);
  void setFrom(TheHub&);
};

class DatasetOptions2: public BoxPanel {
  SUPER(DatasetOptions2,BoxPanel)
  Q_OBJECT
public:
  DatasetOptions2(TheHub&);

signals:
  void imageScale(uint);

private:
  QSpinBox  *marginLeft, *marginTop, *marginRight, *marginBottom;
  QSpinBox  *spinImageScale;

  void setFrom(TheHub&);
};

//------------------------------------------------------------------------------

class Dataset: public BoxPanel {
  SUPER(Dataset,BoxPanel)
public:
  Dataset(TheHub&);

  void setImageScale(uint);

private:
  QPixmap makePixmap(core::shp_LensSystem lenses,core::Range rgeIntens);
  void setDataset(core::shp_Dataset);
  void renderDataset();

  core::shp_Dataset dataset;

  ImageWidget *imageWidget;
};

//------------------------------------------------------------------------------
}
#endif
