// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      tabs_images.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "tabs_images.h"
#include "colors.h"
#include "thehub.h"
#include <QPainter>

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class ImageWidget : public QWidget, protected RefHub {
  CLS(ImageWidget) SUPER(QWidget)
public:
  ImageWidget(TheHub&);

  void  setPixmap(QPixmap const&);
  void  setScale();

  QSize sizeHint() const;

protected:
  void resizeEvent(QResizeEvent*);

private:
  qreal   scale_;
  QPixmap original_, scaled_;

  void paintEvent(QPaintEvent*);
};

ImageWidget::ImageWidget(TheHub& hub)
: RefHub(hub), scale_(0)
{
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  connect(hub_.actions.showOverlay, &QAction::toggled, [this]() {
    update();
  });
}

void ImageWidget::setPixmap(QPixmap const& pixmap) {
  original_ = pixmap;
  setScale();
}

void ImageWidget::setScale() {
  scale_ = 0;
  if (isVisible() && !original_.isNull()) {
    auto ws = static_cast<QWidget*>(parent())->size();
    auto is = original_.size();
    scale_ = .9 * qMin(qreal(ws.width()) / is.width(), qreal(ws.height()) / is.height());
  }

  if (original_.isNull() || !(scale_ > 0))
    scaled_ = QPixmap();
  else
    scaled_ = original_.scaled(original_.size() * scale_);

  updateGeometry();
  update();
}

QSize ImageWidget::sizeHint() const {
  return scaled_.size() + QSize(2, 2);
}

void ImageWidget::resizeEvent(QResizeEvent* e) {
  super::resizeEvent(e);
  setScale();
}

void ImageWidget::paintEvent(QPaintEvent*) {
  QPainter painter(this);

  // frame
  painter.setPen(Qt::black);
  painter.drawRect(rect().adjusted(0, 0, -1, -1));

  // image
  painter.drawPixmap(1, 1, scaled_);

  // overlay follows
  if (!hub_.actions.showOverlay->isChecked())
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

TabsImages::TabsImages(TheHub& hub) : super(hub) {
  auto& actions = hub_.actions;

  {
    auto& box = addTab("Image", Qt::Horizontal).box();

    auto vb = vbox();
    box.addLayout(vb);

    vb->addWidget(iconButton(hub.actions.fixedIntenImageScale));
    vb->addWidget(iconButton(hub_.actions.showOverlay));
    vb->addStretch(1);


    auto grid = gridLayout();
    // overlapping
    grid->addWidget((dataImageWidget_ = new ImageWidget(hub_)), 0, 0, Qt::AlignCenter);
    grid->addWidget((spinN  = spinCell(4,1)),                   0, 0, Qt::AlignHCenter | Qt::AlignTop);

    box.addLayout(grid, 1);

    connect(spinN, slot(QSpinBox,valueChanged,int), [this](int i) {
      n = to_u(qMax(0, i-1));
      render();
    });
  }
  {
    auto& box = addTab("Correction", Qt::Horizontal).box();

    auto vb = vbox();
    box.addLayout(vb);

    vb->addWidget(iconButton(hub.actions.fixedIntenImageScale));
    vb->addWidget(iconButton(hub_.actions.showOverlay));
    vb->addStretch(1);

    box.addWidget((corrImageWidget_ = new ImageWidget(hub_)),
                   1, Qt::AlignCenter);
  }

  connect(actions.enableCorr, &QAction::toggled, [this](bool) {
    render();
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

QPixmap TabsImages::makeBlankPixmap() {
  auto size = hub_.imageSize();

  QPixmap pixmap(to_i(size.w), to_i(size.h));
  pixmap.fill(Qt::white);

  return pixmap;
}

QPixmap TabsImages::makePixmap(calc::shp_ImageLens imageLens) {
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

void TabsImages::setDataset(data::shp_Dataset dataset) {
  dataset_ = dataset;
  render();
}

void TabsImages::render() {
  {
    pint by = hub_.datasetsGroupedBy();
    bool on = by > 1;
    spinN->setEnabled(on);

    n = qMin(n, by - 1);
    spinN->setValue(to_i(n+1));

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
