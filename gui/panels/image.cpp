#include "image.h"
#include "mainwin.h"
#include <QResizeEvent>
#include <QPainter>
#include <QPainter>
#include <QAction>

namespace panel {

ImageWidget::ImageWidget(Image& image_)
: image(image_), upDown(false), leftRight(false), turnDegrees(0), showOverlay(false) {
  setMinimumSize(16,16);  // so it does not completely disappear
}

QSize ImageWidget::sizeHint() const {
  QSize s = size();
  lastHeight = s.height();
  // together with the resizeEvent keeps the widget square if possible TODO better solution?
  s.setWidth(lastHeight);
  s.setHeight(super::sizeHint().height());
  return QSize(lastHeight,lastHeight);
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

void ImageWidget::resizeEvent(QResizeEvent* e) {
  super::resizeEvent(e);

  QRect r = geometry();
  if (r.height() != r.width()) {
    auto m = qMin(r.height(),r.width());
    r.setWidth(m); r.setWidth(m);
    setGeometry(r);
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

      transform.translate(w/2,h/2);

      transform.rotate(turnDegrees);
      if (upDown)     transform.scale(1,-1);
      if (leftRight)  transform.scale(-1,1);

      transform.translate(-w/2,-h/2);
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
  auto cut = image.getCut();
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

Image::Image(MainWin& mainWin_): super(mainWin_,"",Qt::Horizontal), dataset(nullptr), globalNorm(false) {

  auto grid = gridLayout();
  box->addLayout(grid);
  box->addStretch(INT_MAX);

  grid->addWidget(imageWidget = new ImageWidget(*this),0,0);

  auto hb = hbox();
  grid->addLayout(hb,1,0);

  hb->addWidget(check("gl. normalize",mainWin.actImagesGlobalNorm));

  hb->addWidget(icon(":/icon/top"));
  hb->addWidget((cutTop = spinCell(0,999)));
  hb->addWidget(icon(":/icon/bottom"));
  hb->addWidget((cutBottom = spinCell(0,999)));
  hb->addWidget(icon(":/icon/left"));
  hb->addWidget((cutLeft = spinCell(0,999)));
  hb->addWidget(icon(":/icon/right"));
  hb->addWidget((cutRight = spinCell(0,999)));

  hb->addWidget(iconButton(mainWin.actImagesLink));
  hb->addStretch();

  hb->addWidget(iconButton(mainWin.actImagesEye));

  auto vb = vbox();
  grid->addLayout(vb,0,1);

  vb->addStretch();
  vb->addWidget(iconButton(mainWin.actImagesUpDown));
  vb->addWidget(iconButton(mainWin.actImagesLeftRight));
  vb->addWidget(iconButton(mainWin.actImagesTurnRight));
  vb->addWidget(iconButton(mainWin.actImagesTurnLeft));

  auto setCutFromGui = [this](bool topLeft, int value){
    if (mainWin.actImagesLink->isChecked())
      mainWin.session.setImageCut(topLeft, true, value, value, value, value);
    else
      mainWin.session.setImageCut(topLeft, false, cutTop->value(), cutBottom->value(), cutLeft->value(), cutRight->value());
  };

  connect(cutTop, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [setCutFromGui](int value) {
    setCutFromGui(true,value);
  });

  connect(cutBottom, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [setCutFromGui](int value) {
    setCutFromGui(false,value);
  });

  connect(cutLeft, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [setCutFromGui](int value) {
    setCutFromGui(true,value);
  });

  connect(cutRight, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [setCutFromGui](int value) {
    setCutFromGui(false,value);
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

  connect(mainWin.actImagesEye, &QAction::toggled, [this](bool on) {
    imageWidget->setShowOverlay(on);
  });

  mainWin.actImagesEye->setChecked(true);

  connect(mainWin.actImagesGlobalNorm, &QAction::toggled, [this](bool on) {
    globalNorm = on;
    setDataset(dataset);
  });

  connect(&mainWin.session, &Session::datasetSelected, [this](pcCoreDataset dataset) {
    setDataset(dataset);
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

void Image::setDataset(pcCoreDataset dataset_) {
  dataset = dataset_;
  QPixmap pixMap;
  if (dataset) {
    auto image = dataset->getImage();
    pixMap = image.pixmap(globalNorm ? dataset->getFile().maximumIntensity() : image.maximumIntensity());
  }
  imageWidget->setPixmap(pixMap);
}

const Session::imagecut_t &Image::getCut() const {
  return mainWin.session.getImageCut();
}

}

// eof
