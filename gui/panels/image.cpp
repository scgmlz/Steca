#include "image.h"
#include "mainwin.h"
#include <QResizeEvent>
#include <QPainter>

namespace panel {

ImageWidget::ImageWidget(Image& image_): image(image_) {
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
  original = pixmap; scaled = QPixmap();
  update();
}

void ImageWidget::resizeEvent(QResizeEvent* e) {
  super::resizeEvent(e);
  if (lastHeight!=height()) {
    updateGeometry();
    scaled = QPixmap();
    update();
  }
}

void ImageWidget::paintEvent(QPaintEvent*) {
  if (scaled.isNull() && !original.isNull()) {
    scaled = original.scaled(width()-2,height()-2,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    scale.setX((qreal)scaled.width()  / original.width());
    scale.setY((qreal)scaled.height() / original.height());
  }

  QPainter painter(this);

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

  painter.setPen(Qt::green);
  painter.drawRect(r);
}

//------------------------------------------------------------------------------

Image::Image(MainWin& mainWin): super(mainWin,"",Qt::Horizontal) {

  auto v1 = vbox();
  box->addLayout(v1);

  v1->addWidget(imageWidget = new ImageWidget(*this));

  auto v2 = vbox();
  box->addLayout(v2);

  v2->addWidget(label("Top:"));
  v2->addWidget((cutTop = spinCell()));
  v2->addWidget(label("Bottom:"));
  v2->addWidget((cutBottom = spinCell()));
  v2->addWidget(label("Left:"));
  v2->addWidget((cutLeft = spinCell()));
  v2->addWidget(label("Right:"));
  v2->addWidget((cutRight = spinCell()));

  connect(cutTop, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int) {
    setCutFromGui();
  });

  connect(cutBottom, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int) {
    setCutFromGui();
  });

  connect(cutLeft, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int) {
    setCutFromGui();
  });

  connect(cutRight, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int) {
    setCutFromGui();
  });

  v2->addWidget(iconButton(mainWin.actImagesLink));
  v2->addWidget(iconButton(mainWin.actImagesEye));

  v2->addStretch();

  v2->addWidget(iconButton(mainWin.actImagesUpDown));
  v2->addWidget(iconButton(mainWin.actImagesLeftRight));
  v2->addWidget(iconButton(mainWin.actImagesTurnRight));
  v2->addWidget(iconButton(mainWin.actImagesTurnLeft));

  box->addStretch();

  connect(&mainWin.session, &Session::datasetSelected, [&](pcCoreDataset dataset) {
    imageWidget->setPixmap(dataset ? pixmapFromCoreImage(dataset->getImage()) : QPixmap());
  });
}

QPixmap Image::pixmapFromCoreImage(core::Image const& coreImage) {
  int count = coreImage.dataCount();
  if (count < 1) return QPixmap();

  QSize const &size = coreImage.getSize();
  uint  width = size.width(), height = size.height();

  int maximum = coreImage.intensity(0);
  for_i(count) maximum = qMax(maximum,coreImage.intensity(i));

  QImage image(size, QImage::Format_RGB32);

  for (uint y = 0; y < height; ++y) {
    for (uint x = 0; x < width; ++x) {
      double intens = (double)coreImage.intensity(x,y) / (double)maximum;
      if (intens < 0.25) {
        image.setPixel(x, y, qRgb((int)floor(0xff * intens * 4), 0, 0));
        continue;
      }
      if (intens < 0.5) {
        image.setPixel(x, y, qRgb(0xff, (int)floor(0xff * (intens - 0.25) * 4), 0));
        continue;
      }
      if (intens < 0.75) {
        image.setPixel(x, y, qRgb(0xff - (int)floor(0xff * (intens - 0.5) * 4), 0xff,
                                  (int)floor(0xff * (intens - 0.5) * 4)));
        continue;
      } else
        image.setPixel(x, y, qRgb((int)floor(0xff * (intens - 0.75) * 4), 0xff, 0xff));
    }
  }

  return QPixmap::fromImage(image);
}

const Session::imagecut_t &Image::getCut() const {
  return mainWin.session.getImageCut();
}

void Image::setCutFromGui() const {
  mainWin.session.setImageCut(cutLeft->value(), cutTop->value(), cutRight->value(), cutBottom->value());
}

}

// eof
