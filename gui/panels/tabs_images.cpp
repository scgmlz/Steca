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
#include "thehub.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

TabsImages::TabsImages(TheHub& hub) : super(hub) {
  {
    auto& box = addTab("Image").box();
    box.addWidget(iconButton(hub.actions.fixedIntenImageScale));
    box.addWidget(iconButton(hub_.actions.showOverlay));

    imageScale_ = new QSlider(Qt::Vertical);
    imageScale_->setRange(-5, +2);
    imageScale_->setTickPosition(QSlider::TicksRight);
//    connect(sliderImageScale_, slot(QSlider,valueChanged,int), [this](int value) {
//      preal scale(1);

//      if (value > 0)
//        scale = preal(value + 1);
//      else if (value < 0)
//        scale = preal(1. / (-value + 1));

//      emit imageScale(scale);
//    });
//    onSigSessionCleared([this]() {
//      sliderImageScale_->setValue(sliderImageScale_->minimum());
//    });
  }
  {
    auto &tab = addTab("Correction");
  }
}

//------------------------------------------------------------------------------
}}
// eof
