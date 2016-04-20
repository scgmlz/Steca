// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      panel_dataset.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "panel_dataset.h"
#include "thehub.h"

#include <QScrollArea>
#include <QPainter>
#include <QAction>

namespace gui { namespace panel {
//------------------------------------------------------------------------------

DatasetView::DatasetView(TheHub& hub): super(hub), model_(hub.datasetViewModel) {
  setModel(&model_);

  connect(&hub_, &TheHub::datasetsChanged, [this]() {
    model_.signalReset();
     setCurrentIndex(model_.index(0,0));
  });

  connect(&model_, &QAbstractItemModel::modelReset, [this]() {
    for_i (model_.columnCount())
      resizeColumnToContents(i);
  });
}

void DatasetView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);

  auto indexes = selected.indexes();
  hub_.tellSelectedDataset(indexes.isEmpty()
    ? core::shp_Dataset()
    : model_.data(indexes.first(), Model::GetDatasetRole).value<core::shp_Dataset>());
}

//------------------------------------------------------------------------------

DockDatasets::DockDatasets(TheHub& hub)
: super("Datasets","dock-datasets",Qt::Vertical) {
  box_->addWidget((datasetView_ = new DatasetView(hub)));

  auto h = hbox();
  box_->addLayout(h);

  h->addWidget(label("Combine:"));
  h->addWidget(spinCell(4,1));
}

//------------------------------------------------------------------------------

DockDatasetInfo::DockDatasetInfo(TheHub& hub)
: super("Dataset info", "dock-dataset-info", Qt::Vertical), RefHub(hub) {

  using Dataset     = core::Dataset;
  using shp_Dataset = core::shp_Dataset;

  box_->setMargin(0);

  auto scrollArea = new QScrollArea;
  box_->addWidget(scrollArea);

  scrollArea->setFrameStyle(QFrame::NoFrame);

  for_i (Dataset::numAttributes())
    metaInfo_.append(models::CheckedInfo(Dataset::attributeTag(i)));

  scrollArea->setWidget((info_ = new Info(metaInfo_)));

  for_i (Dataset::numAttributes())
    metaInfo_[i].cb->setToolTip("Show value in Datasets list");

  connect(&hub_, &TheHub::datasetSelected, [this](shp_Dataset dataset) {
    for_i (Dataset::numAttributes())
      metaInfo_[i].setText(dataset ? dataset->attributeStrValue(i) : EMPTY_STR);
  });

  for (auto &item: metaInfo_) {
    connect(item.cb, &QCheckBox::clicked, this, [this]() {
      hub_.datasetViewModel.showMetaInfo(metaInfo_);
    });
  }
}

DockDatasetInfo::Info::Info(models::checkedinfo_vec& metaInfo) {
  setLayout((grid_ = gridLayout()));

  grid_->setSpacing(-1);

  for (auto &item: metaInfo) {
    int row = grid_->rowCount();
    grid_->addWidget(label(item.tag),                    row, 0);
    grid_->addWidget((item.cb       = check(EMPTY_STR)), row, 1);
    grid_->addWidget((item.infoText = readCell(16)), row, 2);
  }
}

//------------------------------------------------------------------------------

ImageWidget::ImageWidget(TheHub& hub,Dataset& dataset_)
: RefHub(hub), dataset_(dataset_), showOverlay_(false), scale_(1) {
  setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}

void ImageWidget::setPixmap(QPixmap const& pixmap) {
  original_ = pixmap;
  setScale(scale_);
}

void ImageWidget::setShowOverlay(bool on) {
  showOverlay_ = on;
  update();
}

void ImageWidget::setScale(uint scale) {
  ASSERT(scale > 0)
  scale_ = scale;

  scaled_ = original_.isNull() ? original_ : original_.scaled(original_.size()*scale_);

  updateGeometry();
  update();
}

QSize ImageWidget::sizeHint() const {
  return scaled_.size() + QSize(2,2);
}

void ImageWidget::paintEvent(QPaintEvent*) {
  QPainter painter(this);

  // frame
  painter.setPen(Qt::black);
  painter.drawRect(rect().adjusted(0,0,-1,-1));

  // image
  painter.drawPixmap(1,1,scaled_);

  // overlay follows
  if (!showOverlay_) return;

  // cut
  auto margins = hub_.imageCut();
  QRect r = rect()
    .adjusted(1,1,-2,-2)
    .adjusted(scale_*margins.left,  scale_*margins.top,
             -scale_*margins.right,-scale_*margins.bottom);

  painter.setPen(Qt::lightGray);
  painter.drawRect(r);
}

//------------------------------------------------------------------------------

DatasetOptions1::DatasetOptions1(TheHub& hub)
: super(EMPTY_STR,hub,Qt::Vertical) {

  box_->addWidget(label("Beam offset"));
  auto ho = hbox();
  box_->addLayout(ho);

  ho->addWidget(label("X"));
  ho->addWidget((spinOffsetI_ = spinCell(4,0)));
  spinOffsetI_->setToolTip("Horizontal offset from image center");
  ho->addWidget(label("Y"));
  ho->addWidget((spinOffsetJ_ = spinCell(4,0)));
  spinOffsetJ_->setToolTip("Vertical offset from image center");
  ho->addWidget(label("pix"));
  ho->addWidget(iconButton(hub_.actions.hasBeamOffset));
  ho->addStretch();

  box_->addWidget(label("Detector"));
  auto gd = gridLayout();
  box_->addLayout(gd);

  gd->addWidget((spinDistance_ = spinCell(6,core::Geometry::MIN_DETECTOR_DISTANCE)),    0,0);
  spinDistance_->setToolTip("Sample to detector distance");
  gd->addWidget(label("distance mm"),                         0,1);
  gd->addWidget((spinPixelSize_ = spinCell(6,core::Geometry::MIN_DETECTOR_PIXEL_SIZE)), 1,0);
  spinPixelSize_->setSingleStep(.1);
  spinPixelSize_->setToolTip("Physical pixel size");
  gd->addWidget(label("pixel size mm"),                       1,1);
  gd->setColumnStretch(2,1);

  box_->addWidget(label("Normalization"));
  auto vn = vbox();
  box_->addLayout(vn);

  str_lst options = core::Lens::normStrLst();

  vn->addWidget(comboNormType_ = comboBox(options));
  box_->addStretch();

  connect(&hub_, &TheHub::geometryChanged, [this]() {
    setFrom(hub_);
  });

  auto setEnabled = [this]() {
    bool on = hub_.actions.hasBeamOffset->isChecked();
    spinOffsetI_->setEnabled(on);
    spinOffsetJ_->setEnabled(on);
  };

  setEnabled();

  connect(hub_.actions.hasBeamOffset, &QAction::toggled, [this,setEnabled]() {
    setEnabled();
    setTo(hub_);
  });

  connect(spinOffsetI_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this]() {
    setTo(hub_);
  });

  connect(spinOffsetJ_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this]() {
    setTo(hub_);
  });

  connect(spinDistance_, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this]() {
    setTo(hub_);
  });

  connect(spinPixelSize_, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this]() {
    setTo(hub_);
  });

  connect(comboNormType_, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),[this](int index) {
    hub_.setNorm((core::Lens::eNorm)index);
  });
}

void DatasetOptions1::setTo(TheHub& hub) {
  hub.setGeometry(
    spinDistance_->value(), spinPixelSize_->value(),
    hub.actions.hasBeamOffset->isChecked(),
    core::IJ(spinOffsetI_->value(),spinOffsetJ_->value()));
}

void DatasetOptions1::setFrom(TheHub& hub) {
  auto const& g = hub.geometry();

  hub.actions.hasBeamOffset->setChecked(g.isMidPixOffset);
  spinOffsetI_->setValue(g.midPixOffset.i);
  spinOffsetJ_->setValue(g.midPixOffset.j);

  spinDistance_->setValue(g.detectorDistance);
  spinPixelSize_->setValue(g.pixSize);
}

static str const GROUP_OPTIONS("Options");
static str const KEY_IMAGE_SCALE("image_scale");

static str const GROUP_BEAM("Beam");
static str const KEY_IS_OFFSET("is_offset");
static str const KEY_OFFSET_X("offset_x");
static str const KEY_OFFSET_Y("offset_y");

static str const GROUP_DETECTOR("Detector");
static str const KEY_DISTANCE("distance");
static str const KEY_PIXEL_SIZE("pixel_size");


//------------------------------------------------------------------------------

DatasetOptions2::DatasetOptions2(TheHub& hub)
: super (EMPTY_STR,hub,Qt::Vertical) {

  box_->addWidget(label("Image"));
  auto hb = hbox();
  box_->addLayout(hb);

  hb->addWidget(iconButton(hub_.actions.rotateImage));
  hb->addSpacing(5);
  hb->addWidget(iconButton(hub_.actions.mirrorImage));
  hb->addSpacing(5);
  hb->addWidget(iconButton(hub_.actions.fixedIntensityDisplay));
  hb->addStretch();

  auto sc = hbox();
  box_->addLayout(sc);
  sc->addWidget(label("Scaling"));
  sc->addSpacing(5);
  sc->addWidget((spinImageScale_ = spinCell(4,1,4)));
  spinImageScale_->setToolTip("Image scale");
  sc->addStretch();

  box_->addWidget(label("Cut"));
  auto gc = gridLayout();
  box_->addLayout(gc);

  gc->addWidget(icon(":/icon/cutTopU"),             0,0);
  gc->addWidget((marginTop_ = spinCell(4,0)),           0,1);
  marginTop_->setToolTip("Top cut");
  gc->addWidget(icon(":/icon/cutBottomU"),          0,2);
  gc->addWidget((marginBottom_ = spinCell(4,0)),        0,3);
  marginBottom_->setToolTip("Bottom cut");

  gc->addWidget(iconButton(hub_.actions.linkCuts),   0,5);
  gc->addWidget(iconButton(hub_.actions.showCut), 1,5);

  gc->addWidget(icon(":/icon/cutLeftU"),            1,0);
  gc->addWidget((marginLeft_ = spinCell(4,0)),          1,1);
  marginLeft_->setToolTip("Left cut");
  gc->addWidget(icon(":/icon/cutRightU"),           1,2);
  gc->addWidget((marginRight_ = spinCell(4,0)),         1,3);
  marginRight_->setToolTip("Right cut");
  gc->setColumnStretch(4,1);

  box_->addStretch();

  auto setImageCut = [this](bool topLeft, int value) {
    if (hub_.actions.linkCuts->isChecked())
      hub_.setImageCut(topLeft, true, core::ImageCut(value,value,value,value));
    else
      hub_.setImageCut(topLeft, false, core::ImageCut(marginLeft_->value(), marginTop_->value(), marginRight_->value(), marginBottom_->value()));
  };

  connect(marginLeft_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [setImageCut](int value) {
    setImageCut(true,value);
  });

  connect(marginTop_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [setImageCut](int value) {
    setImageCut(true,value);
  });

  connect(marginRight_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [setImageCut](int value) {
    setImageCut(false,value);
  });

  connect(marginBottom_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [setImageCut](int value) {
    setImageCut(false,value);
  });

  connect(&hub_, &TheHub::geometryChanged, [this]() {
    setFrom(hub_);
  });

  connect(spinImageScale_, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int scale) {
    emit imageScale(scale);
  });
}

void DatasetOptions2::setFrom(TheHub& hub) {
  auto margins = hub.imageCut();

  marginLeft_   ->setValue(margins.left);
  marginTop_    ->setValue(margins.top);
  marginRight_  ->setValue(margins.right);
  marginBottom_ ->setValue(margins.bottom);
}

//------------------------------------------------------------------------------

Dataset::Dataset(TheHub& hub)
: super(hub), dataset_(nullptr) {

  auto &actions = hub_.actions;
  actions.showCut->setChecked(true);

  {
    auto &tab = addTab("Data",Qt::Vertical);
    tab.box_->addWidget(dataImageWidget_ = new ImageWidget(hub_,*this),0,Qt::AlignCenter);
  }

  {
    auto &tab = addTab("Corr.",Qt::Vertical);
    tab.box_->addWidget(corrImageWidget_ = new ImageWidget(hub_,*this),0,Qt::AlignCenter);
  }

  connect(actions.enableCorr, &QAction::toggled, [this](bool) {
    render();
  });

  connect(actions.showCut, &QAction::toggled, [this](bool on) {
    dataImageWidget_->setShowOverlay(on);
    corrImageWidget_->setShowOverlay(on);
  });

  connect(&hub_, &TheHub::displayChange, [this]() {
    render();
  });

  connect(&hub_, &TheHub::datasetSelected, [this](core::shp_Dataset dataset) {
    setDataset(dataset);
  });

  connect(&hub_, &TheHub::geometryChanged, [this]() {
    render();
  });
}

void Dataset::setImageScale(uint scale) {
  dataImageWidget_->setScale(scale);
  corrImageWidget_->setScale(scale);
}

QPixmap Dataset::makePixmap(core::shp_ImageLens lens) {
  QPixmap pixmap;
  auto size = lens->size();
  auto rgeInten = lens->rgeInten(); // REVIEW also global

  if (!size.isEmpty()) {
    qreal maxIntens = rgeInten.max;
    if (maxIntens <= 0) maxIntens = 1;  // sanity

    QImage qimage(size,QImage::Format_RGB32);

    for_ij (size.width(), size.height()) {
      qreal intens = lens->inten(i,j) / maxIntens;

      QRgb rgb;
      if (qIsNaN(intens))
        rgb = qRgb(0x00,0xff,0xff);
      else if (intens < 0.25)
        rgb = qRgb(0xff * intens * 4, 0, 0);
      else if (intens < 0.5)
        rgb = qRgb(0xff, 0xff * (intens - 0.25) * 4, 0);
      else if (intens < 0.75)
        rgb = qRgb(0xff - (0xff * (intens - 0.5) * 4), 0xff, (0xff * (intens - 0.5) * 4));
      else
        rgb = qRgb(0xff * (intens - 0.75) * 4, 0xff, 0xff);

      qimage.setPixel(i,j,rgb);
    }

    pixmap = QPixmap::fromImage(qimage);
  }

  return pixmap;
}

void Dataset::setDataset(core::shp_Dataset dataset) {
  dataset_ = dataset;
  render();
}

void Dataset::render() {
  {
    QPixmap pixMap;
    if (dataset_) {
      auto lens = hub_.lensNoCut(*dataset_);
      pixMap = makePixmap(lens);
    }
    dataImageWidget_->setPixmap(pixMap);
  }

  {
    QPixmap pixMap;
    auto lens = hub_.lensNoCut(hub_.corrImage());
    pixMap = makePixmap(lens);
    corrImageWidget_->setPixmap(pixMap);
  }
}

//------------------------------------------------------------------------------
}}
// eof
