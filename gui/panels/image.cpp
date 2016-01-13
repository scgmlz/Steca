#include "image.h"
#include "mainwin.h"
#include <QResizeEvent>
#include <QPainter>
#include <QPainter>
#include <QAction>

namespace panel {

ImageWidget::ImageWidget(Image& image_)
: image(image_)
, upDown(false), leftRight(false), turnDegrees(0)
, retransform(true) {
}

QSize ImageWidget::sizeHint() const {
  QSize s = size();
  lastHeight = s.height();
  // together with the resizeEvent keeps the widget square if possible TODO better solution?
  s.setWidth(lastHeight);
  s.setHeight(super::sizeHint().height());
  return s;
}

void ImageWidget::setPixmap(QPixmap const& pixmap) {
  original = pixmap;
  update();
}

void ImageWidget::setUpDown(bool on) {
  upDown = on;
  update();
}

void ImageWidget::setLeftRight(bool on) {
  leftRight = on;
  update();
}

void ImageWidget::setTurn(int degrees) {
  turnDegrees = degrees;
  update();
}

void ImageWidget::resizeEvent(QResizeEvent* e) {
  super::resizeEvent(e);
  if (lastHeight!=height()) {
    updateGeometry();
    update();
  }
}

void ImageWidget::paintEvent(QPaintEvent*) {
  if (retransform) {
    retransform = false;

    scaled = original.scaled(width()-2,height()-2,Qt::IgnoreAspectRatio,Qt::FastTransformation);

    auto h = scaled.height(), w = scaled.width();

    scale.setX((qreal)w  / original.width());
    scale.setY((qreal)h / original.height());

    transform = QTransform();
    transform.translate(w/2,h/2);

    if (upDown)     transform.scale(1,-1);
    if (leftRight)  transform.scale(-1,1);
    transform.rotate(turnDegrees);

    transform.translate(-w/2,-h/2);
  }

  QPainter painter(this); painter.setTransform(transform);

  QRect r = rect();
  r.adjust(0,0,-1,-1);

  // frame
  painter.setPen(Qt::black);
  painter.drawRect(r);

  // image
  painter.drawPixmap(1,1,scaled);

  // cut
  auto cut = image.getCut();
  r.adjust(qRound(scale.x()*cut.left),  qRound(scale.y()*cut.top),
          -qRound(scale.x()*cut.right),-qRound(scale.y()*cut.bottom));

  painter.setPen(Qt::lightGray);
  painter.drawRect(r);
}

void ImageWidget::update() {
  retransform = true;
  super::update();
}

//------------------------------------------------------------------------------

Image::Image(MainWin& mainWin_): super(mainWin_,"",Qt::Horizontal) {

  auto v1 = vbox();
  box->addLayout(v1);

  v1->addWidget(imageWidget = new ImageWidget(*this));

  auto v2 = vbox();
  box->addLayout(v2);

  v2->addWidget(label("Top:"));
  v2->addWidget((cutTop = spinCell(0,999)));
  v2->addWidget(label("Bottom:"));
  v2->addWidget((cutBottom = spinCell(0,999)));
  v2->addWidget(label("Left:"));
  v2->addWidget((cutLeft = spinCell(0,999)));
  v2->addWidget(label("Right:"));
  v2->addWidget((cutRight = spinCell(0,999)));

  auto setCutFromGui = [this](bool topLeft){
    mainWin.session.setImageCut(topLeft, cutTop->value(), cutBottom->value(), cutLeft->value(), cutRight->value());
  };

  connect(cutTop, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [setCutFromGui](int) {
    setCutFromGui(true);
  });

  connect(cutBottom, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [setCutFromGui](int) {
    setCutFromGui(false);
  });

  connect(cutLeft, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [setCutFromGui](int) {
    setCutFromGui(true);
  });

  connect(cutRight, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [setCutFromGui](int) {
    setCutFromGui(false);
  });

  connect(&mainWin.session, &Session::imageCutChanged, [this]() {
    // set GUI from cut values
    auto cut = mainWin.session.getImageCut();
    cutTop    ->setValue(cut.top);
    cutBottom ->setValue(cut.bottom);
    cutLeft   ->setValue(cut.left);
    cutRight  ->setValue(cut.right);

    imageWidget->update();
  });

  v2->addWidget(iconButton(mainWin.actImagesLink));
  v2->addWidget(iconButton(mainWin.actImagesEye));
  v2->addWidget(iconButton(mainWin.actImagesGlobalNorm));

  v2->addStretch();

  v2->addWidget(iconButton(mainWin.actImagesUpDown));
  v2->addWidget(iconButton(mainWin.actImagesLeftRight));
  v2->addWidget(iconButton(mainWin.actImagesTurnRight));
  v2->addWidget(iconButton(mainWin.actImagesTurnLeft));

  box->addStretch();

  connect(&mainWin.session, &Session::datasetSelected, [this](pcCoreDataset dataset) {
    QPixmap pixMap;
    if (dataset) {
      auto image = dataset->getImage();
      pixMap = pixmapFromCoreImage(image,image.maximumIntensity());
    }
    imageWidget->setPixmap(pixMap);
  });

  connect(mainWin.actImagesUpDown, &QAction::toggled, [this](bool on) {
    imageWidget->setUpDown(on);
  });

  connect(mainWin.actImagesLeftRight, &QAction::toggled, [this](bool on) {
    imageWidget->setLeftRight(on);
  });

  // the two buttons work almost as radio buttons, in a way
  // to uncheck one, press the other one
  auto setTurn = [](ImageWidget *imageWidget, QAction *thisAct, QAction *thatAct, int degrees) {
    bool on = !thatAct->isChecked();
    thisAct->setChecked(on);
    thatAct->setChecked(false);
    imageWidget->setTurn(on ? degrees : 0);
  };

  connect(mainWin.actImagesTurnRight, &QAction::triggered, [this,&setTurn]() {
    setTurn(imageWidget, mainWin.actImagesTurnRight, mainWin.actImagesTurnLeft, +90);
  });

  connect(mainWin.actImagesTurnLeft, &QAction::triggered, [this,&setTurn]() {
    setTurn(imageWidget, mainWin.actImagesTurnLeft, mainWin.actImagesTurnRight, -90);
  });
}

QPixmap Image::pixmapFromCoreImage(core::Image const& coreImage, int maximumIntensity) {
  int count = coreImage.dataCount();
  if (count < 1) return QPixmap();

  QSize const &size = coreImage.getSize();
  uint  width = size.width(), height = size.height();
  ASSERT(width>0 && height>0) // true because count >= 1

  if (maximumIntensity <= 0) maximumIntensity = 1;  // sanity
  qreal const maximum = maximumIntensity;

  QImage image(size, QImage::Format_RGB32);

  for (uint y = 0; y < height; ++y) {
    for (uint x = 0; x < width; ++x) {
      qreal intens = (qreal)coreImage.intensity(x,y) / maximum;

      QRgb rgb;
      if (intens < 0.25)
        rgb = qRgb(floor(0xff * intens * 4), 0, 0);
      else if (intens < 0.5)
        rgb = qRgb(0xff, floor(0xff * (intens - 0.25) * 4), 0);
      else if (intens < 0.75)
        rgb = qRgb(0xff - floor(0xff * (intens - 0.5) * 4), 0xff, floor(0xff * (intens - 0.5) * 4));
      else
        rgb = qRgb((int)floor(0xff * (intens - 0.75) * 4), 0xff, 0xff);

      image.setPixel(x, y, rgb);
    }
  }

  return QPixmap::fromImage(image);
}

const Session::imagecut_t &Image::getCut() const {
  return mainWin.session.getImageCut();
}

}

// eof
