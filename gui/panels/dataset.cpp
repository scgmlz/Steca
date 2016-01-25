#include "dataset.h"
#include "mainwin.h"
#include <QResizeEvent>
#include <QPainter>
#include <QPainter>
#include <QAction>

namespace panel {

ImageWidget::ImageWidget(Dataset& dataset_)
: dataset(dataset_), showOverlay(false) {
  setMinimumSize(16,16);  // so it does not completely disappear
}

void ImageWidget::setPixmap(QPixmap const& pixmap) {
  original = pixmap;
  update();
}

void ImageWidget::setShowOverlay(bool on) {
  showOverlay = on;
  update();
}

QSize ImageWidget::sizeHint() const {
  lastHeight = height();
  return QSize(lastHeight,lastHeight);
}

void ImageWidget::resizeEvent(QResizeEvent* e) {
  super::resizeEvent(e);
  auto h = height();
  if (h != lastHeight) {
    auto r = geometry();
    r.setWidth(h);
    setGeometry(r);
    updateGeometry();
  }
}

void ImageWidget::paintEvent(QPaintEvent*) {
  if (lastPaintSize!=size()) {
    lastPaintSize!=size();

    if (original.isNull()) {
      scaled = original;
      scale.setX(0); scale.setY(0);
    } else {
      // retransform
      scaled = original.scaled(width()-2,height()-2,Qt::IgnoreAspectRatio,Qt::FastTransformation);

      auto h = scaled.height(), w = scaled.width();

      scale.setX((qreal)w  / original.width());
      scale.setY((qreal)h  / original.height());
    }
  }

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

void ImageWidget::update() {
  lastPaintSize = QSize();
  super::update();
}

//------------------------------------------------------------------------------

Dataset::Dataset(MainWin& mainWin_, Session& session_)
: super("",mainWin_,session_,Qt::Vertical), dataset(nullptr), globalNorm(false) {
  auto bb = hbox(); // (b)ottom (b)uttons

  bb->addWidget(check("gl. normalize",mainWin.actImagesGlobalNorm));
  bb->addWidget(icon(":/icon/top"));
  bb->addWidget((cutTop = spinCell(4,0)));
  bb->addWidget(icon(":/icon/bottom"));
  bb->addWidget((cutBottom = spinCell(4,0)));
  bb->addWidget(icon(":/icon/left"));
  bb->addWidget((cutLeft = spinCell(4,0)));
  bb->addWidget(icon(":/icon/right"));
  bb->addWidget((cutRight = spinCell(4,0)));
  bb->addWidget(iconButton(mainWin.actImagesLink));
  bb->addStretch();
  bb->addWidget(iconButton(mainWin.actImagesEye));

  auto sb = vbox(); // (s)ide   (b)uttons

  sb->addStretch();
  sb->addWidget(iconButton(mainWin.actImagesUpDown));
  sb->addWidget(iconButton(mainWin.actImagesLeftRight));
  sb->addWidget(iconButton(mainWin.actImagesRotateClock));
  sb->addWidget(iconButton(mainWin.actImagesRotateCounter));

  auto hb = hbox();
  hb->addWidget(imageWidget = new ImageWidget(*this),0,0);
  hb->addLayout(sb);

  box->addLayout(hb);
  box->addLayout(bb);

  auto setImageCut = [this](bool topLeft, int value) {
    if (mainWin.actImagesLink->isChecked())
      session.setImageCut(topLeft, true, Session::imagecut_t(value,value,value,value));
    else
      session.setImageCut(topLeft, false, Session::imagecut_t(cutTop->value(), cutBottom->value(), cutLeft->value(), cutRight->value()));
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

  connect(&session_, &Session::imageCutChanged, [this]() {
    // set GUI from cut values
    auto cut = session.getImageCut();
    cutTop    ->setValue(cut.top);
    cutBottom ->setValue(cut.bottom);
    cutLeft   ->setValue(cut.left);
    cutRight  ->setValue(cut.right);

    imageWidget->update();
  });

  connect(mainWin.actImagesEye, &QAction::toggled, [this](bool on) {
    imageWidget->setShowOverlay(on);
  });

  mainWin.actImagesEye->setChecked(true);

  connect(mainWin.actImagesGlobalNorm, &QAction::toggled, [this](bool on) {
    globalNorm = on;
    refresh();
  });

  connect(&session, &Session::datasetSelected, [this](pcCoreDataset dataset) {
    setDataset(dataset);
  });

  connect(mainWin.actImagesUpDown, &QAction::toggled, [this](bool on) {
    session.setUpDown(on);
    refresh();
  });

  connect(mainWin.actImagesLeftRight, &QAction::toggled, [this](bool on) {
    session.setLeftRight(on);
    refresh();
  });

  // the two buttons work, in a way, together
  auto setTurn = [](QAction *clicked, QAction *other) {
    bool on = clicked->isChecked() && !other->isChecked();
    other->setChecked(false);
    clicked->setChecked(on);
    return on;
  };

  connect(mainWin.actImagesRotateClock, &QAction::triggered, [this,&setTurn]() {
    bool on = setTurn(mainWin.actImagesRotateClock, mainWin.actImagesRotateCounter);
    session.setTurnClock(on);
    refresh();
  });

  connect(mainWin.actImagesRotateCounter, &QAction::triggered, [this,&setTurn]() {
    bool on = setTurn(mainWin.actImagesRotateCounter, mainWin.actImagesRotateClock);
    session.setTurnCounter(on);
    refresh();
  });
}

QPixmap Dataset::makePixmap(core::Image const& image, core::Image::intensity_t maxIntensity) {

  QPixmap pixmap;

  uint size = image.getSize();

  if (0 < size) {
    qreal mi = maxIntensity;
    if (mi <= 0) mi = 1;  // sanity

    QImage qimage(QSize(size,size), QImage::Format_RGB32);

    for (uint y = 0; y < size; ++y) {
      for (uint x = 0; x < size; ++x) {
        qreal intens = image.intensity(session,x,y) / mi;

        QRgb rgb;
        if (intens < 0.25)
          rgb = qRgb(floor(0xff * intens * 4), 0, 0);
        else if (intens < 0.5)
          rgb = qRgb(0xff, floor(0xff * (intens - 0.25) * 4), 0);
        else if (intens < 0.75)
          rgb = qRgb(0xff - floor(0xff * (intens - 0.5) * 4), 0xff, floor(0xff * (intens - 0.5) * 4));
        else
          rgb = qRgb((int)floor(0xff * (intens - 0.75) * 4), 0xff, 0xff);

        qimage.setPixel(x, y, rgb);
      }
    }

    pixmap = QPixmap::fromImage(qimage);
  }

  return pixmap;
}

void Dataset::setDataset(pcCoreDataset dataset_) {
  dataset = dataset_;
  QPixmap pixMap;
  if (dataset) {
    auto image = dataset->getImage();
    pixMap = makePixmap(image, globalNorm ? dataset->getDatasets().getMaximumIntensity() : image.maximumIntensity());
  }
  imageWidget->setPixmap(pixMap);
}

}

// eof
