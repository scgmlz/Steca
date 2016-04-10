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

DatasetView::DatasetView(TheHub& theHub): super(theHub), _model(theHub.datasetViewModel) {
  setModel(&_model);

  connect(&theHub, &TheHub::fileSelected, [this](core::shp_File coreFile) {
    _model.setFile(coreFile);
    setCurrentIndex(_model.index(0,0));
  });

  connect(&_model, &QAbstractItemModel::modelReset, [this]() {
    for_i (_model.columnCount())
      resizeColumnToContents(i);
  });
}

void DatasetView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);

  auto indexes = selected.indexes();
  _theHub.setSelectedDataset(indexes.isEmpty()
    ? core::shp_Dataset()
    : _model.data(indexes.first(), Model::GetDatasetRole).value<core::shp_Dataset>());
}

//------------------------------------------------------------------------------

DockDatasets::DockDatasets(TheHub& theHub)
: super("Datasets","dock-datasets",Qt::Vertical) {
  _box->addWidget((_datasetView = new DatasetView(theHub)));

  auto h = hbox();
  _box->addLayout(h);

  h->addWidget(label("Combine:"));
  h->addWidget(spinCell(4,1));
}

//------------------------------------------------------------------------------

DockDatasetInfo::DockDatasetInfo(TheHub& theHub_)
: super("Dataset info", "dock-dataset-info", Qt::Vertical), RefHub(theHub_) {

  using Dataset     = core::Dataset;
  using shp_Dataset = core::shp_Dataset;

  _box->setMargin(0);

  auto scrollArea = new QScrollArea;
  _box->addWidget(scrollArea);

  scrollArea->setFrameStyle(QFrame::NoFrame);

  for_i (Dataset::numAttributes())
    _metaInfo.append(models::CheckedInfo(Dataset::attributeTag(i)));

  scrollArea->setWidget((_info = new Info(_metaInfo)));

  for_i (Dataset::numAttributes())
    _metaInfo[i].cb->setToolTip("Show value in Datasets list");

  connect(&_theHub, &TheHub::datasetSelected, [this](shp_Dataset dataset) {
    for_i (Dataset::numAttributes())
      _metaInfo[i].setText(dataset ? dataset->attributeStrValue(i) : EMPTY_STR);
  });

  for (auto &item: _metaInfo) {
    connect(item.cb, &QCheckBox::clicked, this, [this]() {
      _theHub.datasetViewModel.showMetaInfo(_metaInfo);
    });
  }
}

DockDatasetInfo::Info::Info(models::checkedinfo_vec& metaInfo) {
  setLayout((_grid = gridLayout()));

  _grid->setSpacing(-1);

  for (auto &item: metaInfo) {
    int row = _grid->rowCount();
    _grid->addWidget(label(item.tag),                    row, 0);
    _grid->addWidget((item.cb       = check(EMPTY_STR)), row, 1);
    _grid->addWidget((item.infoText = readCell(16)), row, 2);
  }
}

//------------------------------------------------------------------------------

ImageWidget::ImageWidget(TheHub& theHub,Dataset& dataset_)
: RefHub(theHub), _dataset(dataset_), _showOverlay(false), _scale(1) {
  setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}

void ImageWidget::setPixmap(QPixmap const& pixmap) {
  _original = pixmap;
  setScale(_scale);
}

void ImageWidget::setShowOverlay(bool on) {
  _showOverlay = on;
  update();
}

void ImageWidget::setScale(uint scale_) {
  ASSERT(scale_ > 0)
  _scale = scale_;

  _scaled = _original.isNull() ? _original : _original.scaled(_original.size()*_scale);

  updateGeometry();
  update();
}

QSize ImageWidget::sizeHint() const {
  return _scaled.size() + QSize(2,2);
}

void ImageWidget::paintEvent(QPaintEvent*) {
  QPainter painter(this);

  // frame
  painter.setPen(Qt::black);
  painter.drawRect(rect().adjusted(0,0,-1,-1));

  // image
  painter.drawPixmap(1,1,_scaled);

  // overlay follows
  if (!_showOverlay) return;

  // cut
  auto margins = _theHub.imageCut();
  QRect r = rect()
    .adjusted(1,1,-2,-2)
    .adjusted(_scale*margins.left,  _scale*margins.top,
             -_scale*margins.right,-_scale*margins.bottom);

  painter.setPen(Qt::lightGray);
  painter.drawRect(r);
}

//------------------------------------------------------------------------------

DatasetOptions1::DatasetOptions1(TheHub& theHub)
: super(EMPTY_STR,theHub,Qt::Vertical) {

  _box->addWidget(label("Beam offset"));
  auto ho = hbox();
  _box->addLayout(ho);

  ho->addWidget(label("X"));
  ho->addWidget((_spinOffsetI = spinCell(4,0)));
  _spinOffsetI->setToolTip("Horizontal offset from image center");
  ho->addWidget(label("Y"));
  ho->addWidget((_spinOffsetJ = spinCell(4,0)));
  _spinOffsetJ->setToolTip("Vertical offset from image center");
  ho->addWidget(label("pix"));
  ho->addWidget(iconButton(_theHub.actions.hasBeamOffset));
  ho->addStretch();

  _box->addWidget(label("Detector"));
  auto gd = gridLayout();
  _box->addLayout(gd);

  gd->addWidget((_spinDistance = spinCell(6,core::Geometry::MIN_DETECTOR_DISTANCE)),    0,0);
  _spinDistance->setToolTip("Sample to detector distance");
  gd->addWidget(label("distance mm"),                         0,1);
  gd->addWidget((_spinPixelSize = spinCell(6,core::Geometry::MIN_DETECTOR_PIXEL_SIZE)), 1,0);
  _spinPixelSize->setSingleStep(.1);
  _spinPixelSize->setToolTip("Physical pixel size");
  gd->addWidget(label("pixel size mm"),                       1,1);
  gd->setColumnStretch(2,1);

  _box->addWidget(label("Normalization"));
  auto vn = vbox();
  _box->addLayout(vn);

  str_lst options = core::Lens::normStrLst();

  vn->addWidget(_comboNormType = comboBox(options));
  _box->addStretch();

  connect(&_theHub, &TheHub::geometryChanged, [this]() {
    setFrom(_theHub);
  });

  auto setEnabled = [this]() {
    bool on = _theHub.actions.hasBeamOffset->isChecked();
    _spinOffsetI->setEnabled(on);
    _spinOffsetJ->setEnabled(on);
  };

  setEnabled();

  connect(_theHub.actions.hasBeamOffset, &QAction::toggled, [this,setEnabled]() {
    setEnabled();
    setTo(_theHub);
  });

  connect(_spinOffsetI, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this]() {
    setTo(_theHub);
  });

  connect(_spinOffsetJ, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this]() {
    setTo(_theHub);
  });

  connect(_spinDistance, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this]() {
    setTo(_theHub);
  });

  connect(_spinPixelSize, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this]() {
    setTo(_theHub);
  });

  connect(_comboNormType, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),[this](int index) {
    _theHub.setNorm((core::Lens::eNorm)index);
  });
}

void DatasetOptions1::setTo(TheHub& theHub) {
  theHub.setGeometry(
    _spinDistance->value(), _spinPixelSize->value(),
    theHub.actions.hasBeamOffset->isChecked(),
    core::IJ(_spinOffsetI->value(),_spinOffsetJ->value()));
}

void DatasetOptions1::setFrom(TheHub& theHub) {
  auto const& g = theHub.geometry();

  theHub.actions.hasBeamOffset->setChecked(g.isMidPixOffset);
  _spinOffsetI->setValue(g.midPixOffset.i);
  _spinOffsetJ->setValue(g.midPixOffset.j);

  _spinDistance->setValue(g.detectorDistance);
  _spinPixelSize->setValue(g.pixSize);
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

DatasetOptions2::DatasetOptions2(TheHub& theHub)
: super (EMPTY_STR,theHub,Qt::Vertical) {

  _box->addWidget(label("Image"));
  auto hb = hbox();
  _box->addLayout(hb);

  hb->addWidget(iconButton(_theHub.actions.rotateImage));
  hb->addSpacing(5);
  hb->addWidget(iconButton(_theHub.actions.mirrorImage));
  hb->addSpacing(5);
  hb->addWidget(iconButton(_theHub.actions.fixedIntensityDisplay));
  hb->addStretch();

  auto sc = hbox();
  _box->addLayout(sc);
  sc->addWidget(label("Scaling"));
  sc->addSpacing(5);
  sc->addWidget((_spinImageScale = spinCell(4,1,4)));
  _spinImageScale->setToolTip("Image scale");
  sc->addStretch();

  _box->addWidget(label("Cut"));
  auto gc = gridLayout();
  _box->addLayout(gc);

  gc->addWidget(icon(":/icon/cutTopU"),             0,0);
  gc->addWidget((_marginTop = spinCell(4,0)),           0,1);
  _marginTop->setToolTip("Top cut");
  gc->addWidget(icon(":/icon/cutBottomU"),          0,2);
  gc->addWidget((_marginBottom = spinCell(4,0)),        0,3);
  _marginBottom->setToolTip("Bottom cut");

  gc->addWidget(iconButton(_theHub.actions.linkCuts),   0,5);
  gc->addWidget(iconButton(_theHub.actions.showCut), 1,5);

  gc->addWidget(icon(":/icon/cutLeftU"),            1,0);
  gc->addWidget((_marginLeft = spinCell(4,0)),          1,1);
  _marginLeft->setToolTip("Left cut");
  gc->addWidget(icon(":/icon/cutRightU"),           1,2);
  gc->addWidget((_marginRight = spinCell(4,0)),         1,3);
  _marginRight->setToolTip("Right cut");
  gc->setColumnStretch(4,1);

  _box->addStretch();

  auto setImageCut = [this](bool topLeft, int value) {
    if (_theHub.actions.linkCuts->isChecked())
      _theHub.setImageCut(topLeft, true, core::ImageCut(value,value,value,value));
    else
      _theHub.setImageCut(topLeft, false, core::ImageCut(_marginLeft->value(), _marginTop->value(), _marginRight->value(), _marginBottom->value()));
  };

  connect(_marginLeft, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [setImageCut](int value) {
    setImageCut(true,value);
  });

  connect(_marginTop, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [setImageCut](int value) {
    setImageCut(true,value);
  });

  connect(_marginRight, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [setImageCut](int value) {
    setImageCut(false,value);
  });

  connect(_marginBottom, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [setImageCut](int value) {
    setImageCut(false,value);
  });

  connect(&_theHub, &TheHub::geometryChanged, [this]() {
    setFrom(_theHub);
  });

  connect(_spinImageScale, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int scale) {
    emit imageScale(scale);
  });
}

void DatasetOptions2::setFrom(TheHub& theHub) {
  auto margins = theHub.imageCut();

  _marginLeft   ->setValue(margins.left);
  _marginTop    ->setValue(margins.top);
  _marginRight  ->setValue(margins.right);
  _marginBottom ->setValue(margins.bottom);
}

//------------------------------------------------------------------------------

Dataset::Dataset(TheHub& theHub)
: super(EMPTY_STR,theHub,Qt::Vertical), _dataset(nullptr) {

  auto &actions = _theHub.actions;

  _box->addWidget(_imageWidget = new ImageWidget(_theHub,*this),0,Qt::AlignCenter);

  connect(actions.enableCorr, &QAction::toggled, [this](bool) {
    renderDataset();
  });

  connect(actions.showCut, &QAction::toggled, [this](bool on) {
    _imageWidget->setShowOverlay(on);
  });

  actions.showCut->setChecked(true);

  connect(&_theHub, &TheHub::displayChange, [this]() {
    renderDataset();
  });

  connect(&_theHub, &TheHub::datasetSelected, [this](core::shp_Dataset dataset) {
    setDataset(dataset);
  });

  connect(&_theHub, &TheHub::geometryChanged, [this]() {
    renderDataset();
  });
}

void Dataset::setImageScale(uint scale) {
  _imageWidget->setScale(scale);
}

QPixmap Dataset::makePixmap(core::shp_Lens lens) {
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

void Dataset::setDataset(core::shp_Dataset dataset_) {
  _dataset = dataset_;
  renderDataset();
}

void Dataset::renderDataset() {
  QPixmap pixMap;
  if (_dataset) {
    auto lens = _theHub.lensNoCut(*_dataset);
    pixMap = makePixmap(lens);
  }
  _imageWidget->setPixmap(pixMap);
}

//------------------------------------------------------------------------------
}}
// eof
