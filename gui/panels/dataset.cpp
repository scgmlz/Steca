#include "dataset.h"
#include "mainwin.h"
#include <QResizeEvent>
#include <QPainter>
#include <QPainter>
#include <QAction>

namespace panel {

ImageWidget::ImageWidget(Dataset& image_)
: image(image_), upDown(false), leftRight(false), turnDegrees(0), showOverlay(false) {
  setMinimumSize(16,16);  // so it does not completely disappear
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

    transform = QTransform();

    if (original.isNull()) {
      scaled = original;
      scale.setX(0); scale.setY(0);
    } else {
      // retransform
      scaled = original.scaled(width()-2,height()-2,Qt::IgnoreAspectRatio,Qt::FastTransformation);

      auto h = scaled.height(), w = scaled.width();

      scale.setX((qreal)w  / original.width());
      scale.setY((qreal)h  / original.height());

      int transX = w/2, transY = h/2;

      // TODO if transformed, there is a light (one-pix?) column on the side of the image
      transform.translate(transX,transY);

      transform.rotate(turnDegrees);
      if (leftRight) {
        transform.scale(-1,1);
        transX += 1; // adjustment; TODO verify - it is still not quite perfect
      }
      if (upDown) {
        transform.scale(1,-1);
        transY += 1; // adjustment; TODO verify - it is still not quite perfect
      }

      transform.translate(-transX,-transY);
    }
  }

  QPainter painter(this); painter.setTransform(transform);

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
  auto cut = image.getSession().getImageCut();
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
  sb->addWidget(iconButton(mainWin.actImagesTurnRight));
  sb->addWidget(iconButton(mainWin.actImagesTurnLeft));

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
    setDataset(dataset);
  });

  connect(&session, &Session::datasetSelected, [this](pcCoreDataset dataset) {
    setDataset(dataset);
  });

  connect(mainWin.actImagesUpDown, &QAction::toggled, [this](bool on) {
    imageWidget->setUpDown(on);
  });

  connect(mainWin.actImagesLeftRight, &QAction::toggled, [this](bool on) {
    imageWidget->setLeftRight(on);
  });

  // the two buttons work, in a way, together
  auto setTurn = [](ImageWidget *imageWidget, QAction *clicked, QAction *other, int degrees) {
    bool on = clicked->isChecked() && !other->isChecked();
    other->setChecked(false);
    clicked->setChecked(on);
    imageWidget->setTurn(on ? degrees : 0);
  };

  connect(mainWin.actImagesTurnRight, &QAction::triggered, [this,&setTurn]() {
    setTurn(imageWidget, mainWin.actImagesTurnRight, mainWin.actImagesTurnLeft, +90);
  });

  connect(mainWin.actImagesTurnLeft, &QAction::triggered, [this,&setTurn]() {
    setTurn(imageWidget, mainWin.actImagesTurnLeft, mainWin.actImagesTurnRight, -90);
  });
}

void Dataset::setDataset(pcCoreDataset dataset_) {
  dataset = dataset_;
  QPixmap pixMap;
  if (dataset) {
    auto image = dataset->getImage();
    pixMap = image.pixmap(globalNorm ? dataset->getDatasets().getMaximumIntensity() : image.maximumIntensity());
  }
  imageWidget->setPixmap(pixMap);
}

}

// eof
