// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      panel_image.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "panel_image.h"
#include "colors.h"
#include "thehub.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>

namespace gui { namespace panel {
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

void ImageWidget::setScale(preal scale) {
  if (original_.isNull()) {
    scale_  = scale;
    scaled_ = original_;
  } else {
    qreal const M = .7;
    auto r = QApplication::desktop()->screenGeometry();
    qreal maxHeight = r.height() * M;
    qreal maxWidth  = r.width()  * M;

    auto o = original_.size();
    auto maxScale = qMin(maxWidth / o.width(), maxHeight / o.height());

    scale_  = qMin(scale, preal(maxScale));
    scaled_ = original_.scaled(original_.size() * scale_);
  }

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
  QRect r = rect().adjusted(1, 1, -2, -2)
                  .adjusted(qRound(scale_ * margins.left),   qRound(scale_ * margins.top),
                       -qRound(scale_ * margins.right), -qRound(scale_ * margins.bottom));
  painter.setPen(Qt::lightGray);
  painter.drawRect(r);

  // cross
  auto off = hub_.geometry().midPixOffset;
  auto c = r.center();
  auto x = qRound(c.x() + scale_ * off.i);
  auto y = qRound(c.y() + scale_ * off.j);
  painter.drawLine(x, r.top(), x, r.bottom());
  painter.drawLine(r.left(), y, r.right(), y);
}

//------------------------------------------------------------------------------

ImagePanel::ImagePanel(TheHub& hub) : super(hub), dataset_(nullptr) {

  auto& actions = hub_.actions;
  actions.showOverlay->setChecked(true);

  {
    auto& tab = addTab("Data", Qt::Horizontal);
    auto& box = tab.box();

    box.addWidget((spinN  = spinCell(4,1)));
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

void ImagePanel::setImageScale(preal scale) {
  dataImageWidget_->setScale(scale);
  corrImageWidget_->setScale(scale);
}

QPixmap ImagePanel::makeBlankPixmap() {
  auto size = hub_.imageSize();

  QPixmap pixmap(to_i(size.w), to_i(size.h));
  pixmap.fill(Qt::white);

  return pixmap;
}

QPixmap ImagePanel::makePixmap(calc::shp_ImageLens imageLens) {
  QPixmap pixmap;
  auto size     = imageLens->size();
  auto rgeInten = imageLens->rgeInten(hub_.isFixedIntenImageScale());

  if (!size.isEmpty()) {
    QImage image(QSize(to_i(size.w), to_i(size.h)), QImage::Format_RGB32);

    inten_t maxInten = inten_t(rgeInten.max);
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

    QPixmap pixMap = dataset_
        ? makePixmap(hub_.plainImageLens(dataset_->at(n)->image()))
        : makeBlankPixmap();

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
