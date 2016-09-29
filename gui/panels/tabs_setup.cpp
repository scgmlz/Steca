// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      tabs_setup.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "tabs_setup.h"
#include "thehub.h"
#include "typ/typ_geometry.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

TabsSetup::TabsSetup(TheHub& hub) : super(hub) {
  {
    auto& box = addTab("Geometry", Qt::Vertical).box();

    onSigGeometryChanged([this]() {
      setFromHub();
    });

    // widgets

    detDistance_  = spinCell(6, typ::Geometry::MIN_DETECTOR_DISTANCE);
    detPixelSize_ = spinCell(6, typ::Geometry::MIN_DETECTOR_PIXEL_SIZE);

    connect(detDistance_, slot(QDoubleSpinBox,valueChanged,double), [this]() {
      setToHub();
    });

    connect(detPixelSize_, slot(QDoubleSpinBox,valueChanged,double), [this]() {
      setToHub();
    });

    beamOffsetI_  = spinCell(6, 0);
    beamOffsetJ_  = spinCell(6, 0);

    connect(beamOffsetI_, slot(QSpinBox,valueChanged,int), [this]() {
      setToHub();
    });

    connect(beamOffsetJ_, slot(QSpinBox,valueChanged,int), [this]() {
      setToHub();
    });

    marginLeft_   = spinCell(4, 0);
    marginTop_    = spinCell(4, 0);
    marginRight_  = spinCell(4, 0);
    marginBottom_ = spinCell(4, 0);

    auto setImageCut = [this](bool topLeft, int value) {
      EXPECT(value >= 0)
      if (hub_.actions.linkCuts->isChecked())
        hub_.setImageCut(topLeft, true, typ::ImageCut(
                         to_u(value), to_u(value), to_u(value), to_u(value)));
      else
        hub_.setImageCut(topLeft, false,
                         typ::ImageCut(to_u(marginLeft_->value()),  to_u(marginTop_->value()),
                                       to_u(marginRight_->value()), to_u(marginBottom_->value())));
    };

    connect(marginLeft_, slot(QSpinBox,valueChanged,int), [setImageCut](int value) {
      setImageCut(true, value);
    });

    connect(marginTop_, slot(QSpinBox,valueChanged,int), [setImageCut](int value) {
      setImageCut(true, value);
    });

    connect(marginRight_, slot(QSpinBox,valueChanged,int), [setImageCut](int value) {
      setImageCut(false, value);
    });

    connect(marginBottom_, slot(QSpinBox,valueChanged,int), [setImageCut](int value) {
      setImageCut(false, value);
    });

    // layout

    auto grid = gridLayout(); int row = 0;

    auto add = [&grid, &row](QVector<QWidget*> const& ws, int left = 1) {
      int i = 0, cnt = ws.count();

      auto box = hbox(); box->addStretch(1);
      while (i < left)
        box->addWidget(ws.at(i++));
      grid->addLayout(box, row, 0);

      box = hbox();
      while (i < cnt)
        box->addWidget(ws.at(i++));
      grid->addLayout(box, row, 1);

      box->addStretch(1);

      row++;
    };

    add({ label("detector distance"), detDistance_,  label("mm") });
    add({ label("pixel size"),        detPixelSize_, label("mm") });

    add({ label("beam offset X"),     beamOffsetI_,  label("pix") });
    add({ label("Y"),                 beamOffsetJ_,  label("pix") });

    add({ label("image rotate"),      iconButton(hub_.actions.rotateImage),
                                      label("mirror"), iconButton(hub_.actions.mirrorImage) });

    add({ label("margin"),            icon(":/icon/cutLeft"),  marginLeft_,
                                      icon(":/icon/cutRight"), marginRight_,
                                      iconButton(hub_.actions.linkCuts) }, 2);
    add({ icon(":/icon/cutTop"),      marginTop_,
                                      icon(":/icon/cutBottom"), marginBottom_});

    grid->addColumnStretch(1);

    box.addLayout(grid);
    box.addStretch();
  }
  {
    auto& tab = addTab("Background", Qt::Vertical);
  }
  {
    auto& tab = addTab("Reflections", Qt::Vertical);
  }
}

void TabsSetup::setToHub() {
  hub_.setGeometry(
    preal(qMax(qreal(typ::Geometry::MIN_DETECTOR_DISTANCE),   detDistance_->value())),
    preal(qMax(qreal(typ::Geometry::MIN_DETECTOR_PIXEL_SIZE), detPixelSize_->value())),
    typ::IJ(beamOffsetI_->value(), beamOffsetJ_->value())
  );
}

void TabsSetup::setFromHub() {
  auto& g = hub_.geometry();

  detDistance_->setValue(g.detectorDistance);
  detPixelSize_->setValue(g.pixSize);

  beamOffsetI_->setValue(g.midPixOffset.i);
  beamOffsetJ_->setValue(g.midPixOffset.j);

  auto margins = hub_.imageCut(); // RENAME Cut -> Margin everywhere

  marginLeft_  ->setValue(to_i(margins.left));
  marginTop_   ->setValue(to_i(margins.top));
  marginRight_ ->setValue(to_i(margins.right));
  marginBottom_->setValue(to_i(margins.bottom));
}

//------------------------------------------------------------------------------
}}
// eof
