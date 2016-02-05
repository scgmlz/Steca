#include "dataset.h"
#include "mainwin.h"
#include "session.h"
#include "settings.h"
#include <QPainter>
#include <QAction>

namespace panel {

ImageWidget::ImageWidget(Dataset& dataset_)
: dataset(dataset_), showOverlay(false) {
//  setMinimumSize(16,16);  // so it does not completely disappear
  setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}

void ImageWidget::setPixmap(QPixmap const& pixmap) {
  scaled = original = pixmap;
  updateGeometry();
  update();
}

void ImageWidget::setShowOverlay(bool on) {
  showOverlay = on;
  update();
}

QSize ImageWidget::sizeHint() const {
  return scaled.size();
}

void ImageWidget::resizeEvent(QResizeEvent* e) {
  super::resizeEvent(e);
//  auto h = height();
//  if (h != lastHeight) {
//    auto r = geometry();
//    r.setWidth(h);
//    setGeometry(r);
//    updateGeometry(); // TODO on Mac stays a while, then changes
//  }
}

void ImageWidget::paintEvent(QPaintEvent*) {
//  if (lastPaintSize!=size()) {
//    lastPaintSize = size();

//    if (original.isNull()) {
//      scaled = original;
//      scale.setX(0); scale.setY(0);
//    } else {
//      // retransform
//      scaled = original.scaled(width()-2,height()-2,Qt::IgnoreAspectRatio,Qt::FastTransformation);

//      auto h = scaled.height(), w = scaled.width();

//      scale.setX((qreal)w  / original.width());
//      scale.setY((qreal)h  / original.height());
//    }
//  }

  QPainter painter(this);

  QRect r = rect();
  r.adjust(0,0,-1,-1);

  // frame
  painter.setPen(Qt::black);
  painter.drawRect(r);

  // image
  painter.drawPixmap(1,1,scaled);

  // overlay
  if (!showOverlay) return;

  // cut
  auto cut = dataset.getSession().getImageCut();
  r.adjust(qRound(scale.x()*cut.left),  qRound(scale.y()*cut.top),
          -qRound(scale.x()*cut.right),-qRound(scale.y()*cut.bottom));

  painter.setPen(Qt::lightGray);
  painter.drawRect(r);
}

//------------------------------------------------------------------------------

DatasetOptions::DatasetOptions(MainWin& mainWin_, Session& session_)
: super ("Options",mainWin_,session_,Qt::Vertical) {

  auto hb = hbox();

  hb->addWidget(iconButton(session.actImageRotate));
  hb->addWidget(iconButton(session.actImageMirror));
  hb->addSpacing(1);
  hb->addWidget(iconButton(mainWin.actImagesShowRaw));
  hb->addWidget(iconButton(mainWin.actImageOverlay));
  hb->addStretch();

  auto gc = gridLayout();

  gc->addWidget(icon(":/icon/top"),                 0,0);
  gc->addWidget((cutTop = spinCell(4,0)),           0,1);
  gc->addWidget(icon(":/icon/bottom"),              1,0);
  gc->addWidget((cutBottom = spinCell(4,0)),        1,1);
  gc->addWidget(icon(":/icon/left"),                2,0);
  gc->addWidget((cutLeft = spinCell(4,0)),          2,1);
  gc->addWidget(icon(":/icon/right"),               3,0);
  gc->addWidget((cutRight = spinCell(4,0)),         3,1);
  gc->addWidget(iconButton(mainWin.actImagesLink),  4,0);
  gc->setColumnStretch(2,1);

  box->addLayout(hb);
  box->addLayout(gc);

  box->addWidget(check("gl. norm.",mainWin.actImagesGlobalNorm));

  box->addWidget((checkIsBeamOffset = check("Beamctr. offset")));

  auto go = gridLayout();
  go->addWidget(label("X"),                         0,0);
  go->addWidget((spinOffsetX = spinCell(4,0)),      0,1);
  go->addWidget(label("pix"),                       0,2);
  go->addWidget(label("Y"),                         1,0);
  go->addWidget((spinOffsetY = spinCell(4,0)),      1,1);
  go->addWidget(label("pix"),                       1,2);
  go->setColumnStretch(3,1);

  box->addLayout(go);
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

  connect(&session, &Session::geometryChanged, [this]() {
    auto cut = session.getImageCut();

    cutTop    ->setValue(cut.top);
    cutBottom ->setValue(cut.bottom);
    cutLeft   ->setValue(cut.left);
    cutRight  ->setValue(cut.right);
  });

  auto setEnabled = [this]() {
    bool on = checkIsBeamOffset->isChecked();
    spinOffsetX->setEnabled(on);
    spinOffsetY->setEnabled(on);
  };

  connect(checkIsBeamOffset, &QCheckBox::toggled, [setEnabled]() {
    setEnabled();
  });

  setEnabled();

  connect(checkIsBeamOffset, &QCheckBox::toggled, [this]() {
    setTo(session);
  });

  connect(spinOffsetX, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this]() {
    setTo(session);
  });

  connect(spinOffsetY, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this]() {
    setTo(session);
  });
}

void DatasetOptions::setTo(Session& session) {
  session.setBeamGeometry(
    checkIsBeamOffset->isChecked(),
    QPoint(spinOffsetX->value(),spinOffsetY->value()));
}

void DatasetOptions::setFrom(Session& session) {
  auto const& g = session.getGeometry();

  checkIsBeamOffset->setChecked(g.hasBeamOffset);
  spinOffsetX->setValue(g.middlePixOffset.x());
  spinOffsetY->setValue(g.middlePixOffset.y());
}

static str GROUP_BEAM("Beam");
static str KEY_IS_OFFSET("is_offset");
static str KEY_OFFSET_X("offset_x");
static str KEY_OFFSET_Y("offset_y");

void DatasetOptions::readSettings(Session& session) {
  Settings s(GROUP_BEAM);
  auto const& g = session.getGeometry();

  s.read(KEY_IS_OFFSET,   checkIsBeamOffset,g.hasBeamOffset);
  s.read(KEY_OFFSET_X,    spinOffsetX,      g.middlePixOffset.x());
  s.read(KEY_OFFSET_Y,    spinOffsetY,      g.middlePixOffset.y());
}

void DatasetOptions::saveSettings() {
  Settings s(GROUP_BEAM);

  s.save(KEY_IS_OFFSET,   checkIsBeamOffset);
  s.save(KEY_OFFSET_X,    spinOffsetX);
  s.save(KEY_OFFSET_Y,    spinOffsetY);
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

QPixmap Dataset::makePixmap(core::Image const& image, core::Range rgeIntens,
                            core::Image* corr) {
  QPixmap pixmap;
  auto size = image.getSize();

  if (!size.isEmpty()) {
    qreal mi = rgeIntens.max;
    if (mi <= 0) mi = 1;  // sanity

    QImage qimage(size,QImage::Format_RGB32);

    for_i(size.height()) {
      auto y = i;
      for_i(size.width()) {
        auto x = i;
        qreal intens = image.intensity(session.imageTransform,x,y) / mi;
        bool isNan = false; // TODO temporary fix
        if (corr) {
          auto factor = corr->intensity(session.imageTransform,x,y);
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
