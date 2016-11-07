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
//! @authors   Rebecca Brydon, Jan Burle, Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "tabs_images.h"
#include "colors.h"
#include "gui_cfg.h"
#include "thehub.h"
#include <QColor>
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
    painter.setPen(Qt::lightGray);

    // cut
    auto cut = hub_.imageCut();
    QRect r = rect.adjusted(-1, -1, 0, 0)
                  .adjusted(qRound(scale_ * cut.left),   qRound(scale_ * cut.top),
                           -qRound(scale_ * cut.right), -qRound(scale_ * cut.bottom));
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
    box.setAlignment(hb, Qt::AlignTop);

    hb->addWidget(iconButton(actions.fixedIntenImage));
    hb->addWidget(iconButton(actions.stepScale));
    hb->addWidget(iconButton(actions.showOverlay));
    hb->addWidget((spinN_ = spinCell(gui_cfg::em4,1)));

    hb->addStretch(1);

    hb->addWidget(iconButton(actions.showGamma));
    hb->addWidget(label("γ count"));
    hb->addWidget((numSlices_ = spinCell(gui_cfg::em4, 0)));
    hb->addWidget(label("#"));
    hb->addWidget((numSlice_  = spinCell(gui_cfg::em4, 1)));

    hb->addWidget(label("min"));
    hb->addWidget((minGamma_ = spinDoubleCell(gui_cfg::em4_2)));
    hb->addWidget(label("max"));
    hb->addWidget((maxGamma_ = spinDoubleCell(gui_cfg::em4_2)));

    minGamma_->setReadOnly(true);
    maxGamma_->setReadOnly(true);

    box.addWidget((dataImageWidget_ = new ImageWidget(hub_)));

    connect(spinN_, slot(QSpinBox,valueChanged,int), [this]() {
      render();
    });

    connect(numSlices_, slot(QSpinBox,valueChanged,int), [this]() {
      render();
    });

    connect(numSlice_, slot(QSpinBox,valueChanged,int), [this]() {
      render();
    });
  }

  {
    auto& tab = addTab("Correction", Qt::Vertical);

    onSigCorrFile([&tab](data::shp_File file) {
      tab.setEnabled(!file.isNull());
    });

    auto& box = tab.box();

    auto hb = hbox();
    box.addLayout(hb);
    box.setAlignment(hb, Qt::AlignTop);

    hb->addWidget(iconButton(actions.fixedIntenImage));
    hb->addWidget(iconButton(actions.stepScale));
    hb->addWidget(iconButton(actions.showOverlay));
    hb->addStretch(1);

    box.addWidget((corrImageWidget_ = new ImageWidget(hub_)));
  }

  connect(actions.enableCorr, &QAction::toggled, [this](bool) {
    render();
  });

  connect(actions.showGamma, &QAction::toggled, [this]() {
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

  render();
}

QPixmap TabsImages::makeBlankPixmap() {
  auto size = hub_.imageSize();

  QPixmap pixmap(to_i(size.w), to_i(size.h));
  pixmap.fill(QColor(0, 0, 0, 0));

  return pixmap;
}

QImage TabsImages::makeImage(typ::Image::rc image) {
  auto imageLens = hub_.plainImageLens(image);

  auto size     = imageLens->size();
  if (size.isEmpty())
    return  QImage();

  QImage im(QSize(to_i(size.w), to_i(size.h)), QImage::Format_RGB32);

  auto rgeInten = imageLens->rgeInten(hub_.isFixedIntenImageScale());
  inten_t maxInten = inten_t(rgeInten.max);

  for_ij (size.w, size.h)
    im.setPixel(to_i(i), to_i(j),
                intenImage(imageLens->imageInten(i, j), maxInten));
  return im;
}

QPixmap TabsImages::makePixmap(typ::Image::rc image) {
  return QPixmap::fromImage(makeImage(image));
}

QPixmap TabsImages::makePixmap(data::OneDataset::rc dataset, gma_rge::rc rgeGma) {
  auto im = makeImage(dataset.image());
  auto angleMap = hub_.angleMap(dataset);

  auto size = im.size();
  for_ij (size.width(), size.height())
    if (rgeGma.contains(angleMap->at(to_u(i),to_u(j)).gma)) {
      auto color = QColor(im.pixel(i,j));
      color.setGreen(qFloor(color.green()*.7 + 255*.3));
      im.setPixel(i,j, color.rgb());
    }

  return QPixmap::fromImage(im);
}

void TabsImages::setDataset(data::shp_Dataset dataset) {
  dataset_ = dataset;
  render();
}

void TabsImages::render() {
  {
    QPixmap pixMap;

    uint nSlices  = to_u(numSlices_->value());
    numSlice_->setMaximum(qMax(1, to_i(nSlices)));
    numSlice_->setEnabled(nSlices > 0);

    if (dataset_) {
      // 1 - based
      uint by = qBound(1u, uint(hub_.datasetsGroupedBy()), dataset_->count());
      uint n  = qBound(1u, to_u(spinN_->value()), by);

      spinN_->setValue(to_i(n));
      spinN_->setEnabled(by > 1);

      lens_ = hub_.datasetLens(*dataset_);

      typ::Range rge;
      if (nSlices > 0) {
        uint nSlice  = qMax(1u, to_u(numSlice_->value()));
        uint iSlice  = nSlice - 1;

        auto rgeGma = lens_->rgeGma();
        auto min    = rgeGma.min;
        auto wn = rgeGma.width() / nSlices;

        rge = gma_rge(min + iSlice * wn, min + (iSlice+1) * wn);

        minGamma_->setValue(rge.min);
        maxGamma_->setValue(rge.max);
      } else {
        rge = typ::Range::infinite();;
        minGamma_->clear();
        maxGamma_->clear();
      }

      hub_.setGammaRange(rge);

      auto oneDataset = dataset_->at(n-1);
      if (hub_.actions.showGamma->isChecked()) {
        pixMap = makePixmap(*oneDataset, rge);
      } else {
        pixMap = makePixmap(oneDataset->image());
      }
    } else {
      spinN_->setEnabled(false);

      pixMap = makeBlankPixmap();
    }

    dataImageWidget_->setPixmap(pixMap);
  }

  {
    QPixmap pixMap = makePixmap(hub_.corrImage());
    corrImageWidget_->setPixmap(pixMap);
  }
}

//------------------------------------------------------------------------------
}}
// eof
