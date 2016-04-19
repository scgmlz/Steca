// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      panel_dataset.h
//! @brief     File selection panel.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef PANEL_DATASET_H
#define PANEL_DATASET_H

#include "panel.h"
#include "core_dataset.h"
#include "models.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class DatasetView: public ListView {
  SUPER(DatasetView,ListView)
public:
  using Model = models::DatasetViewModel;

  DatasetView(TheHub&);

protected:
  void selectionChanged(QItemSelection const&, QItemSelection const&);

private:
  Model &model_;
};

//------------------------------------------------------------------------------

class DockDatasets: public DockWidget {
  SUPER(DockDatasets,DockWidget)
public:
  DockDatasets(TheHub&);
private:
  DatasetView *datasetView_;
};

//------------------------------------------------------------------------------

class DockDatasetInfo: public DockWidget, protected RefHub {
  SUPER(DockDatasetInfo,DockWidget)
public:
  DockDatasetInfo(TheHub&);

private:
  class Info: public QWidget {
  public:
    Info(models::checkedinfo_vec&);
    QGridLayout *grid_;
  };

  Info *info_;
  models::checkedinfo_vec metaInfo_;
};

//------------------------------------------------------------------------------

class Dataset;

class ImageWidget: public QWidget, protected RefHub {
  SUPER(ImageWidget,QWidget)
public:
  ImageWidget(TheHub&,Dataset&);

  void setPixmap(QPixmap const&);
  void setShowOverlay(bool);
  void setScale(uint);

  QSize sizeHint() const;

protected:
  Dataset &dataset_;
  bool showOverlay_;
  QPixmap original_, scaled_;
  uint scale_;

  void paintEvent(QPaintEvent*);
};

//------------------------------------------------------------------------------

class DatasetOptions1: public BoxPanel {
  SUPER(DatasetOptions1,BoxPanel)
  Q_OBJECT
public:
  DatasetOptions1(TheHub&);

private:
  QSpinBox       *spinOffsetI_, *spinOffsetJ_;
  QDoubleSpinBox *spinDistance_, *spinPixelSize_;
  QComboBox      *comboNormType_;

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
  QSpinBox  *marginLeft_, *marginTop_, *marginRight_, *marginBottom_;
  QSpinBox  *spinImageScale_;

  void setFrom(TheHub&);
};

//------------------------------------------------------------------------------

class Dataset: public TabsPanel {
  SUPER(Dataset,TabsPanel)
public:
  Dataset(TheHub&);

  void setImageScale(uint);

private:
  QPixmap makePixmap(core::shp_ImageLens);
  void setDataset(core::shp_Dataset);
  void render();

  core::shp_Dataset dataset_;
  ImageWidget *dataImageWidget_, *corrImageWidget_;
};

//------------------------------------------------------------------------------
}}
#endif // PANEL_DATASET_H
