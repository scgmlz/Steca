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
#include <QResizeEvent>
#include <qmath.h>

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class ImageWidget : public QWidget, protected RefHub {
  CLS(ImageWidget) SUPER(QWidget)
public:
  ImageWidget(TheHub&);

  void setPixmap(QPixmap const&);
  void setScale();

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
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  connect(hub_.actions.showOverlay, &QAction::toggled, [this]() {
    update();
  });

  connect(hub_.actions.stepScale, &QAction::toggled, [this]() {
    setScale();
  });
}

void ImageWidget::setPixmap(QPixmap const& pixmap) {
  original_ = pixmap;
  setScale();
}

void ImageWidget::setScale() {
  if (original_.isNull()) {
    scale_ = 0;
  } else {
    auto sz = size();
    auto os = original_.size();
    scale_  = qMin(qreal(sz.width() - 2) / os.width(), qreal(sz.height() - 2) / os.height());
  }

  if (hub_.actions.stepScale->isChecked() && scale_ > 0)
    scale_ = (scale_ >= 1) ? qFloor(scale_) : 1.0 / qCeil(1.0 / scale_);

  if (original_.isNull() || !(scale_ > 0))
    scaled_ = QPixmap();
  else
    scaled_ = original_.scaled(original_.size() * scale_);

  update();
}

void ImageWidget::resizeEvent(QResizeEvent* e) {
  super::resizeEvent(e);
  setScale();
}

void ImageWidget::paintEvent(QPaintEvent*) {
  // paint centered
  auto margin = (size() - scaled_.size()) / 2;
  QRect rect(QPoint(margin.width(), margin.height()), scaled_.size());

  QPainter painter(this);

  // image
  painter.drawPixmap(rect.left(), rect.top(), scaled_);

  // overlay
  if (hub_.actions.showOverlay->isChecked()) {
    // cut
    auto  margins = hub_.imageCut();
    QRect r = rect.adjusted(-1, -1, 0, 0)
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

  // frame
  painter.setPen(Qt::black);
  painter.drawRect(rect.adjusted(-1, -1, 0, 0));
}

//------------------------------------------------------------------------------

TabsImages::TabsImages(TheHub& hub) : super(hub) {
  auto& actions = hub_.actions;

  {
    auto& box = addTab("Image", Qt::Vertical).box();

    auto hb = hbox();
    box.addLayout(hb);

    hb->addWidget(iconButton(actions.fixedIntenImageScale));
    hb->addWidget(iconButton(actions.stepScale));
    hb->addWidget(iconButton(actions.showOverlay));
    hb->addWidget((spinN_ = spinCell(4,1)));
    hb->addStretch(1);
    hb->addWidget(label("γ min"));
    hb->addWidget((minGamma_ = spinCell(6, -180., 180.)));
    hb->addWidget(label("max"));
    hb->addWidget((maxGamma_ = spinCell(6, -180., 180.)));

    box.addWidget((dataImageWidget_ = new ImageWidget(hub_)), 0, 0);

    connect(spinN_, slot(QSpinBox,valueChanged,int), [this](int i) {
      n = to_u(qMax(0, i-1));
      render();
    });
  }

  {
    auto& tab = addTab("Correction", Qt::Horizontal);

    onSigCorrFile([&tab](data::shp_File file) {
      tab.setEnabled(!file.isNull());
    });

    auto& box = tab.box();

    auto g = gridLayout();
    box.addLayout(g);

    g->addWidget((corrImageWidget_ = new ImageWidget(hub_)), 0, 0);

    auto vb = vbox();
    g->addLayout(vb, 0, 0);

    vb->addWidget(iconButton(actions.fixedIntenImageScale));
    vb->addWidget(iconButton(actions.stepScale));
    vb->addWidget(iconButton(actions.showOverlay));
    vb->addStretch(1);
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
  pixmap.fill(QColor(0, 0, 0, 0));

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

  if (dataset_) {
    by = qMin(uint(hub_.datasetsGroupedBy()), dataset_->count());

    lens_ = hub_.datasetLens(*dataset_);

    if (!rgeGma_.isValid()) {
      rgeGma_ = lens_->rgeGma();
      minGamma_->setValue(rgeGma_.min);
      maxGamma_->setValue(rgeGma_.max);
    }
  }

  render();
}

void TabsImages::render() {
  {
    bool on = by > 1;
    spinN_->setEnabled(on);

    QPixmap pixMap;

    if (dataset_) {
      n = qMin(n, by - 1);
      spinN_->setValue(to_i(n+1));
      pixMap = makePixmap(hub_.plainImageLens(dataset_->at(n)->image()));
    } else {
      pixMap = makeBlankPixmap();
    }

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
