// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      panel_dataset.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "panel_dataset.h"
#include "calc/calc_lens.h"
#include "colors.h"
#include "thehub.h"
#include "typ/typ_geometry.h"

#include <QAction>
#include <QPainter>
#include <QScrollArea>

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class DatasetView : public views::ListView {
  CLS(DatasetView) SUPER(views::ListView)
public:
  DatasetView(TheHub&);

protected:
  void currentChanged(QModelIndex const&, QModelIndex const&);

  using Model = models::DatasetsModel;
  Model* model() const { return static_cast<Model*>(super::model()); }
};

DatasetView::DatasetView(TheHub& hub) : super(hub) {
  setModel(&hub.datasetsModel);
  EXPECT(dynamic_cast<Model*>(super::model()))

  onSigDatasetsChanged([this]() {
    tellDatasetSelected(data::shp_Dataset()); // first de-select
    selectRow(0);
  });
}

void DatasetView::currentChanged(QModelIndex const& current,
                                 QModelIndex const& previous) {
  super::currentChanged(current, previous);
  tellDatasetSelected(model()->data(current, Model::GetDatasetRole)
                      .value<data::shp_Dataset>());
}

//------------------------------------------------------------------------------

DockDatasets::DockDatasets(TheHub& hub)
: super("Datasets", "dock-datasets", Qt::Vertical), RefHub(hub)
{
  box_->addWidget((datasetView_ = new DatasetView(hub)));

  auto h = hbox();
  box_->addLayout(h);

  h->addWidget(label("Combine:"));
  h->addWidget(combineDatasets_ = spinCell(4, 1));
  combineDatasets_->setToolTip("Combine and average number of datasets");

  connect(combineDatasets_, slot(QSpinBox,valueChanged,int), [this](int num) {
    hub_.combineDatasetsBy(pint(qMax(1, num)));
  });

  onSigDatasetsChanged([this]() {
    combineDatasets_->setValue(to_i(uint(hub_.datasetsGroupedBy())));
  });
}

//------------------------------------------------------------------------------

DockMetadata::DockMetadata(TheHub& hub)
: super("Metadata", "dock-metadata", Qt::Vertical), RefHub(hub)
{
  using Metadata    = data::Metadata;
  using shp_Dataset = data::shp_Dataset;

  box_->setMargin(0);

  auto scrollArea = new QScrollArea;
  box_->addWidget(scrollArea);

  scrollArea->setFrameStyle(QFrame::NoFrame);

  for_i (Metadata::numAttributes(false))
    metaInfo_.append(models::CheckedInfo(Metadata::attributeTag(i)));

  scrollArea->setWidget((info_ = new Info(metaInfo_)));

  for_i (Metadata::numAttributes(false))
    metaInfo_[i].cb->setToolTip("Show value in Datasets list");

  onSigDatasetSelected([this](shp_Dataset dataset) {
    for_i (Metadata::numAttributes(false))
      metaInfo_[i].setText(dataset ? dataset->metadata()->attributeStrValue(i)
                                   : EMPTY_STR);
  });

  for (auto& item : metaInfo_) {
    connect(item.cb, &QCheckBox::clicked, this, [this]() {
      hub_.datasetsModel.showMetaInfo(metaInfo_);
    });
  }
}

DockMetadata::Info::Info(models::checkedinfo_vec& metaInfo) {
  setLayout((grid_ = gridLayout()));

  grid_->setSpacing(-1);

  for (auto& item : metaInfo) {
    int row = grid_->rowCount();
    grid_->addWidget(label(item.tag), row, 0);
    grid_->addWidget((item.cb = check(EMPTY_STR)), row, 1);
    grid_->addWidget((item.infoText = readCell(16)), row, 2);
  }
}

//------------------------------------------------------------------------------

ImageWidget::ImageWidget(TheHub& hub, ImagePanel& dataset_)
: RefHub(hub), dataset_(dataset_), showOverlay_(true), scale_(1)
{
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
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
  EXPECT(scale > 0)
  scale_ = scale;

  scaled_ = original_.isNull() ? original_
                               : original_.scaled(original_.size() * scale_);

  updateGeometry();
  update();
}

QSize ImageWidget::sizeHint() const {
  return scaled_.size() + QSize(2, 2);
}

void ImageWidget::paintEvent(QPaintEvent*) {
  QPainter painter(this);

  // frame
  painter.setPen(Qt::black);
  painter.drawRect(rect().adjusted(0, 0, -1, -1));

  // image
  painter.drawPixmap(1, 1, scaled_);

  // overlay follows
  if (!showOverlay_)
    return;

  // cut
  auto  margins = hub_.imageCut();
  QRect r = rect()
      .adjusted(1, 1, -2, -2)
      .adjusted(to_i(scale_ * margins.left),   to_i(scale_ * margins.top),
               -to_i(scale_ * margins.right), -to_i(scale_ * margins.bottom));
  painter.setPen(Qt::lightGray);
  painter.drawRect(r);
}

//------------------------------------------------------------------------------

DatasetOptions1::DatasetOptions1(TheHub& hub)
: super(hub, Qt::Vertical)
{
  box_->addWidget(label("Beam offset"));
  auto ho = hbox();
  box_->addLayout(ho);

  ho->addWidget(label("X"));
  ho->addWidget((spinOffsetI_ = spinCell(4, 0)));
  spinOffsetI_->setToolTip("Horizontal offset from image center");
  ho->addWidget(label("Y"));
  ho->addWidget((spinOffsetJ_ = spinCell(4, 0)));
  spinOffsetJ_->setToolTip("Vertical offset from image center");
  ho->addWidget(label("pix"));
  ho->addWidget(iconButton(hub_.actions.hasBeamOffset));
  ho->addStretch();

  box_->addWidget(label("Detector"));
  auto gd = gridLayout();
  box_->addLayout(gd);

  gd->addWidget(
      (spinDistance_ = spinCell(6, typ::Geometry::MIN_DETECTOR_DISTANCE)), 0,
      0);
  spinDistance_->setToolTip("Sample to detector distance");
  gd->addWidget(label("distance mm"), 0, 1);
  gd->addWidget(
      (spinPixelSize_ = spinCell(6, typ::Geometry::MIN_DETECTOR_PIXEL_SIZE)),
      1, 0);
  spinPixelSize_->setSingleStep(.1);
  spinPixelSize_->setToolTip("Physical pixel size");
  gd->addWidget(label("pixel size mm"), 1, 1);
  gd->setColumnStretch(2, 1);

  box_->addStretch();

  onSigGeometryChanged([this]() {
    setFrom(hub_);
  });

  auto setEnabled = [this]() {
    bool on = hub_.actions.hasBeamOffset->isChecked();
    spinOffsetI_->setEnabled(on);
    spinOffsetJ_->setEnabled(on);
  };

  setEnabled();

  connect(hub_.actions.hasBeamOffset, &QAction::toggled, [this, setEnabled]() {
    setEnabled();
    setTo(hub_);
  });

  connect(spinOffsetI_, slot(QSpinBox,valueChanged,int), [this]() {
    setTo(hub_);
  });

  connect(spinOffsetJ_, slot(QSpinBox,valueChanged,int), [this]() {
    setTo(hub_);
  });

  connect(spinDistance_, slot(QDoubleSpinBox,valueChanged,double), [this]() {
    setTo(hub_);
  });

  connect(spinPixelSize_, slot(QDoubleSpinBox,valueChanged,double), [this]() {
    setTo(hub_);
  });
}

void DatasetOptions1::setTo(TheHub& hub) {
  hub.setGeometry(
    preal(qMax(qreal(typ::Geometry::MIN_DETECTOR_DISTANCE),   spinDistance_->value())),
    preal(qMax(qreal(typ::Geometry::MIN_DETECTOR_PIXEL_SIZE), spinPixelSize_->value())),
    hub.actions.hasBeamOffset->isChecked(),
    typ::IJ(spinOffsetI_->value(), spinOffsetJ_->value()));
}

void DatasetOptions1::setFrom(TheHub& hub) {
  auto& g = hub.geometry();

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
: super(hub, Qt::Vertical)
{
  box_->addWidget(label("Image"));
  auto hb = hbox();
  box_->addLayout(hb);

  hb->addWidget(iconButton(hub_.actions.rotateImage));
  hb->addSpacing(5);
  hb->addWidget(iconButton(hub_.actions.mirrorImage));
  hb->addSpacing(5);
  hb->addWidget(iconButton(hub_.actions.fixedIntenImageScale));
  hb->addStretch();

  auto sc = hbox();
  box_->addLayout(sc);
  sc->addWidget(label("Scale"));
  sc->addSpacing(5);
  sc->addWidget((spinImageScale_ = spinCell(4, 1, 4)));
  spinImageScale_->setToolTip("Image scale");
  sc->addStretch();

  box_->addWidget(label("Cut"));
  auto gc = gridLayout();
  box_->addLayout(gc);

  gc->addWidget(icon(":/icon/cutTop"), 0, 0);
  gc->addWidget((marginTop_ = spinCell(4, 0)), 0, 1);
  marginTop_->setToolTip("Top cut");
  gc->addWidget(icon(":/icon/cutBottom"), 0, 2);
  gc->addWidget((marginBottom_ = spinCell(4, 0)), 0, 3);
  marginBottom_->setToolTip("Bottom cut");

  gc->addWidget(iconButton(hub_.actions.linkCuts), 0, 5);
  gc->addWidget(iconButton(hub_.actions.showOverlay), 1, 5);

  gc->addWidget(icon(":/icon/cutLeft"), 1, 0);
  gc->addWidget((marginLeft_ = spinCell(4, 0)), 1, 1);
  marginLeft_->setToolTip("Left cut");
  gc->addWidget(icon(":/icon/cutRight"), 1, 2);
  gc->addWidget((marginRight_ = spinCell(4, 0)), 1, 3);
  marginRight_->setToolTip("Right cut");
  gc->setColumnStretch(4, 1);

  box_->addWidget(label("Normalisation"));
  auto vn = vbox();
  box_->addLayout(vn);

  str_lst options = normStrLst();

  vn->addWidget(comboNormType_ = comboBox(options));

  box_->addStretch();

  auto setImageCut = [this](bool topLeft, int value) {
    EXPECT(value >= 0)
    if (hub_.actions.linkCuts->isChecked())
      hub_.setImageCut(topLeft, true, typ::ImageCut(
                       to_u(value), to_u(value), to_u(value), to_u(value)));
    else
      hub_.setImageCut(topLeft, false,
                       typ::ImageCut(to_u(marginLeft_->value()),  to_u(marginTop_->value()),
                                      to_u(marginRight_->value()), to_u(marginBottom_->value())));
  };

  connect(marginLeft_, slot(QSpinBox,valueChanged,int), [setImageCut](int value) {
    setImageCut(true, value);
  });

  connect(marginTop_, slot(QSpinBox,valueChanged,int), [setImageCut](int value) {
    setImageCut(true, value);
  });

  connect(marginRight_, slot(QSpinBox,valueChanged,int), [setImageCut](int value) {
    setImageCut(false, value);
  });

  connect(marginBottom_, slot(QSpinBox,valueChanged,int), [setImageCut](int value) {
    setImageCut(false, value);
  });

  onSigGeometryChanged([this]() {
    setFrom(hub_);
  });

  connect(spinImageScale_, slot(QSpinBox,valueChanged,int), [this](int scale) {
    EXPECT(scale >= 0)
    emit imageScale(to_u(scale));
  });

  connect(comboNormType_, slot(QComboBox,currentIndexChanged,int), [this](int index) {
    hub_.setNorm(eNorm(index));
  });
}

void DatasetOptions2::setFrom(TheHub& hub) {
  auto margins = hub.imageCut();

  marginLeft_  ->setValue(to_i(margins.left));
  marginTop_   ->setValue(to_i(margins.top));
  marginRight_ ->setValue(to_i(margins.right));
  marginBottom_->setValue(to_i(margins.bottom));
}

//------------------------------------------------------------------------------

ImagePanel::ImagePanel(TheHub& hub) : super(hub), dataset_(nullptr) {

  auto& actions = hub_.actions;
  actions.showOverlay->setChecked(true);

  {
    auto& tab = addTab("Data", Qt::Horizontal);
    auto& box = tab.box();

    box.addWidget((spinN  = spinCell(3,1)));
    box.addWidget((labelN = label("")));
    box.addWidget((dataImageWidget_ = new ImageWidget(hub_, *this)),
                   1, Qt::AlignCenter);

    connect(spinN, slot(QSpinBox,valueChanged,int), [this](int i) {
      n = to_u(qMax(0, i-1));
      render();
    });
  }

  {
    auto& tab = addTab("Corr.");
    tab.box().addWidget(corrImageWidget_ = new ImageWidget(hub_, *this), 0,
                       Qt::AlignCenter);
  }

  connect(actions.enableCorr, &QAction::toggled, [this](bool) {
    render();
  });

  connect(actions.showOverlay, &QAction::toggled, [this](bool on) {
    dataImageWidget_->setShowOverlay(on);
    corrImageWidget_->setShowOverlay(on);
  });

  onSigDisplayChanged([this]() {
    render();
  });

  onSigGeometryChanged([this]() {
    render();
  });

  onSigNormChanged([this]() {
    render();
  });

  onSigDatasetSelected([this](data::shp_Dataset dataset) {
    setDataset(dataset);
  });
}

void ImagePanel::setImageScale(uint scale) {
  dataImageWidget_->setScale(scale);
  corrImageWidget_->setScale(scale);
}

QPixmap ImagePanel::makePixmap(calc::shp_ImageLens imageLens) {
  QPixmap pixmap;
  auto size     = imageLens->size();
  auto rgeInten = imageLens->rgeInten(hub_.isFixedIntenImageScale());

  if (!size.isEmpty()) {
    QImage image(QSize(to_i(size.w), to_i(size.h)), QImage::Format_RGB32);

    qreal maxInten = rgeInten.max;
    for_ij (size.w, size.h)
      image.setPixel(to_i(i), to_i(j),
                     intenImage(imageLens->imageInten(i, j), maxInten));
    pixmap = QPixmap::fromImage(image);
  }

  return pixmap;
}

void ImagePanel::setDataset(data::shp_Dataset dataset) {
  dataset_ = dataset;
  render();
}

void ImagePanel::render() {
  {
    pint by = hub_.datasetsGroupedBy();
    bool on = by > 1;
    spinN->setEnabled(on);

    n = qMin(n, by - 1);
    spinN->setValue(to_i(n+1));

    labelN->setText(str("/%1").arg(by));

    QPixmap pixMap;
    if (dataset_) {
      auto one  = dataset_->at(n);
      auto lens = hub_.plainImageLens(one->image());
      pixMap    = makePixmap(lens);
    }
    dataImageWidget_->setPixmap(pixMap);
  }

  {
    QPixmap pixMap = makePixmap(hub_.plainImageLens(hub_.corrImage()));
    corrImageWidget_->setPixmap(pixMap);
  }
}

//------------------------------------------------------------------------------
}}
// eof
