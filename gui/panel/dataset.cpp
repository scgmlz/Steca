#include "dataset.h"
#include "mainwin.h"
#include "session.h"
#include "settings.h"
#include <QPainter>
#include <QAction>

namespace panel {

ImageWidget::ImageWidget(Dataset& dataset_)
: dataset(dataset_), showOverlay(false), scale(2) {
  setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}

void ImageWidget::setPixmap(QPixmap const& pixmap) {
  original = pixmap;
  setScale(scale);
}

void ImageWidget::setShowOverlay(bool on) {
  showOverlay = on;
  update();
}

void ImageWidget::setScale(uint scale_) {
  ASSERT(scale_ > 0)
  scale = scale_;

  scaled = original.isNull() ? original : original.scaled(original.size()*scale);

  updateGeometry();
  update();
}

QSize ImageWidget::sizeHint() const {
  return scaled.size() + QSize(2,2);
}

void ImageWidget::paintEvent(QPaintEvent*) {
  QPainter painter(this);

  // frame
  painter.setPen(Qt::black);
  painter.drawRect(rect().adjusted(0,0,-1,-1));

  // image
  painter.drawPixmap(1,1,scaled);

  // overlay follows
  if (!showOverlay) return;

  // cut
  auto cut = dataset.getSession().getImageCut();
  QRect r = rect()
    .adjusted(1,1,-2,-2)
    .adjusted(scale*cut.left,scale*cut.top,-scale*cut.right,-scale*cut.bottom);

  painter.setPen(Qt::lightGray);
  painter.drawRect(r);
}

//------------------------------------------------------------------------------

DatasetOptions::DatasetOptions(MainWin& mainWin_, Session& session_)
: super ("Options",mainWin_,session_,Qt::Vertical) {

  // TODO clean up the layout

  auto hb = hbox();

  hb->addWidget(iconButton(session.actImageRotate));
  hb->addWidget(iconButton(session.actImageMirror));
  hb->addSpacing(1);
  hb->addWidget(iconButton(mainWin.actImagesShowRaw));
  hb->addWidget(iconButton(mainWin.actImageOverlay));
  hb->addWidget(iconButton(mainWin.actImagesGlobalNorm));
  hb->addStretch();

  auto sz = hbox();
  sz->addWidget(label("scale"));
  sz->addWidget((spinImageScale = spinCell(4,1,4)));
  sz->addStretch();

  auto gc = gridLayout();

  gc->addWidget(icon(":/icon/top"),                 0,0);
  gc->addWidget((cutTop = spinCell(4,0)),           0,1);
  gc->addWidget(iconButton(mainWin.actImagesLink),  0,2);
  gc->addWidget(icon(":/icon/bottom"),              1,0);
  gc->addWidget((cutBottom = spinCell(4,0)),        1,1);
  gc->addWidget(icon(":/icon/left"),                2,0);
  gc->addWidget((cutLeft = spinCell(4,0)),          2,1);
  gc->addWidget(icon(":/icon/right"),               3,0);
  gc->addWidget((cutRight = spinCell(4,0)),         3,1);
  gc->setColumnStretch(3,1);

  box->addWidget(label("Image"));
  box->addLayout(hb);
  box->addLayout(sz);
  box->addWidget(label("Cut"));
  box->addLayout(gc);

  auto go = gridLayout();
  go->addWidget(iconButton(mainWin.actHasBeamOffset),0,0);
  go->addWidget(label("X"),                         0,1);
  go->addWidget((spinOffsetX = spinCell(4,0)),      0,2);
  go->addWidget(label("pix"),                       0,3);
  go->addWidget(label("Y"),                         1,1);
  go->addWidget((spinOffsetY = spinCell(4,0)),      1,2);
  go->addWidget(label("pix"),                       1,3);
  go->setColumnStretch(3,1);

  box->addWidget(label("Beam offset"));
  box->addLayout(go);

  auto gd = gridLayout();

  gd->addWidget(label("Distance"),                  0,0);
  gd->addWidget((spinDistance = spinCell(6,0.0)),   0,1);
  gd->addWidget(label("mm"),                        0,2);
  gd->addWidget(label("Pixel size"),                1,0);
  gd->addWidget((spinPixelSize = spinCell(6,0.01)), 1,1);
  gd->addWidget(label("mm"),                        1,2);
  gd->setColumnStretch(3,1);

  box->addWidget(label("Detector"));
  box->addLayout(gd);

  box->addStretch();

  auto setImageCut = [this](bool topLeft, int value) {
    if (mainWin.actImagesLink->isChecked())
      session.setImageCut(topLeft, true, core::ImageCut(value,value,value,value));
    else
      session.setImageCut(topLeft, false, core::ImageCut(cutTop->value(), cutBottom->value(), cutLeft->value(), cutRight->value()));
  };

  connect(cutTop, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [setImageCut](int value) {
    setImageCut(true,value);
  });

  connect(cutBottom, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [setImageCut](int value) {
    setImageCut(false,value);
  });

  connect(cutLeft, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [setImageCut](int value) {
    setImageCut(true,value);
  });

  connect(cutRight, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [setImageCut](int value) {
    setImageCut(false,value);
  });

  // TODO separate geometryChanged into more signals?
  connect(&session, &Session::geometryChanged, [this]() {
    setFrom(session);
  });

  auto setEnabled = [this]() {
    bool on = mainWin.actHasBeamOffset->isChecked();
    spinOffsetX->setEnabled(on);
    spinOffsetY->setEnabled(on);
  };

  setEnabled();

  // TODO split setTo() ?
  connect(mainWin.actHasBeamOffset, &QAction::toggled, [this,setEnabled]() {
    setEnabled();
    setTo(session);
  });

  connect(spinOffsetX, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this]() {
    setTo(session);
  });

  connect(spinOffsetY, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this]() {
    setTo(session);
  });

  connect(spinImageScale, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int scale) {
    emit imageScale(scale);
  });

  connect(spinDistance, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this]() {
    setTo(session);
  });

  connect(spinPixelSize, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this]() {
    setTo(session);
  });

  // TODO handle in Session() ?
  connect(&session, &Session::readSettings, [this]() {
    readSettings(session);
  });

  connect(&session, &Session::saveSettings, [this]() {
    saveSettings();
  });
}

void DatasetOptions::setTo(Session& session) {
  session.setGeometry(
    spinDistance->value(), spinPixelSize->value(),
    mainWin.actHasBeamOffset->isChecked(),
    QPoint(spinOffsetX->value(),spinOffsetY->value()));
}

void DatasetOptions::setFrom(Session& session) {
  auto const& g = session.getGeometry();

  mainWin.actHasBeamOffset->setChecked(g.hasBeamOffset);
  spinOffsetX->setValue(g.middlePixOffset.x());
  spinOffsetY->setValue(g.middlePixOffset.y());

  spinDistance->setValue(g.sampleDetectorSpan);
  spinPixelSize->setValue(g.pixSpan);

  auto cut = session.getImageCut();

  cutTop    ->setValue(cut.top);
  cutBottom ->setValue(cut.bottom);
  cutLeft   ->setValue(cut.left);
  cutRight  ->setValue(cut.right);
}

static str GROUP_OPTIONS("Options");
static str KEY_IMAGE_SCALE("image_scale");

static str GROUP_BEAM("Beam");
static str KEY_IS_OFFSET("is_offset");
static str KEY_OFFSET_X("offset_x");
static str KEY_OFFSET_Y("offset_y");

static str GROUP_DETECTOR("Detector");
static str KEY_DISTANCE("distance");
static str KEY_PIXEL_SIZE("pixel_size");

void DatasetOptions::readSettings(Session& session) {
  {
    Settings s(GROUP_OPTIONS);
    s.read(KEY_IMAGE_SCALE, spinImageScale, 4);
  }
  {
    Settings s(GROUP_BEAM);
    auto const& g = session.getGeometry();
    s.read(KEY_IS_OFFSET,   mainWin.actHasBeamOffset,g.hasBeamOffset);
    s.read(KEY_OFFSET_X,    spinOffsetX,      g.middlePixOffset.x());
    s.read(KEY_OFFSET_Y,    spinOffsetY,      g.middlePixOffset.y());
  }
  {
    Settings s(GROUP_DETECTOR);
    auto const& g = session.getGeometry();
    s.read(KEY_DISTANCE,    spinDistance,     g.sampleDetectorSpan);
    s.read(KEY_PIXEL_SIZE,  spinPixelSize,    g.pixSpan);
  }

  emit imageScale(spinImageScale->value()); // TODO review
}

void DatasetOptions::saveSettings() {
  {
    Settings s(GROUP_OPTIONS);
    s.save(KEY_IMAGE_SCALE, spinImageScale);
  }
  {
    Settings s(GROUP_BEAM);
    s.save(KEY_IS_OFFSET,   mainWin.actHasBeamOffset);
    s.save(KEY_OFFSET_X,    spinOffsetX);
    s.save(KEY_OFFSET_Y,    spinOffsetY);
  }
  {
    Settings s(GROUP_DETECTOR);
    s.save(KEY_DISTANCE,    spinDistance);
    s.save(KEY_PIXEL_SIZE,  spinPixelSize);
  }
}

//------------------------------------------------------------------------------

Dataset::Dataset(MainWin& mainWin_, Session& session_)
: super("",mainWin_,session_,Qt::Vertical), dataset(nullptr), globalNorm(false) {

  box->addWidget(imageWidget = new ImageWidget(*this),0,Qt::AlignCenter);

  connect(mainWin.actImagesShowRaw, &QAction::toggled, [this](bool) {
    refresh();
  });

  connect(mainWin.actImageOverlay, &QAction::toggled, [this](bool on) {
    imageWidget->setShowOverlay(on);
  });

  mainWin.actImageOverlay->setChecked(true);

  connect(mainWin.actImagesGlobalNorm, &QAction::toggled, [this](bool on) {
    globalNorm = on;
    refresh();
  });

  connect(&session, &Session::datasetSelected, [this](core::shp_Dataset dataset) {
    setDataset(dataset);
  });

  connect(&session, &Session::geometryChanged, [this]() {
    refresh();
  });
}

void Dataset::setImageScale(uint scale) {
  imageWidget->setScale(scale);
}

QPixmap Dataset::makePixmap(core::Image const& image, core::Range rgeIntens,
                            core::Image* corr) {
  QPixmap pixmap;
  auto size = session.getImageSize();

  if (!size.isEmpty()) {
    qreal mi = rgeIntens.max;
    if (mi <= 0) mi = 1;  // sanity

    QImage qimage(size,QImage::Format_RGB32);

    for_i (size.height()) {
      auto y = i;
      for_i (size.width()) {
        auto x = i;
        qreal intens = image.intensity(session.pixIndex(x,y)) / mi;
        bool isNan = false; // TODO temporary fix
        if (corr) {
          auto factor = corr->intensity(session.pixIndex(x,y));
          if (qIsNaN(factor)) // TODO still actual?
            isNan = true;
          else
            intens *= factor;
        }

        QRgb rgb;
        if (isNan)
          rgb = qRgb(0x00,0xff,0xff);
        else if (intens < 0.25)
          rgb = qRgb(0xff * intens * 4, 0, 0);
        else if (intens < 0.5)
          rgb = qRgb(0xff, 0xff * (intens - 0.25) * 4, 0);
        else if (intens < 0.75)
          rgb = qRgb(0xff - (0xff * (intens - 0.5) * 4), 0xff, (0xff * (intens - 0.5) * 4));
        else
          rgb = qRgb(0xff * (intens - 0.75) * 4, 0xff, 0xff);

        qimage.setPixel(x, y, rgb);
      }
    }

    pixmap = QPixmap::fromImage(qimage);
  }

  return pixmap;
}

void Dataset::setDataset(core::shp_Dataset dataset_) {
  dataset = dataset_;
  QPixmap pixMap;
  if (dataset) {
    auto image = dataset->getImage();
    core::Image *corr = nullptr;
    if (session.hasCorrFile() && !mainWin.actImagesShowRaw->isChecked())
      corr = &session.intensCorrArray;
    pixMap = makePixmap(image, globalNorm ? dataset->getFile().getRgeIntens() : image.getRgeIntens(), corr);
  }
  imageWidget->setPixmap(pixMap);
}

void Dataset::refresh() {
  setDataset(dataset);
}

}

// eof
