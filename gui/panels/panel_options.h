// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      panel_options.h
//! @brief     Options panel.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef PANEL_OPTIONS_H
#define PANEL_OPTIONS_H

#include "panel.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class DatasetOptions1 : public BoxPanel {
  Q_OBJECT
  CLS(DatasetOptions1) SUPER(BoxPanel)
public:
  DatasetOptions1(TheHub&);

private:
  QSpinBox       *spinOffsetI_, *spinOffsetJ_;
  QDoubleSpinBox *spinDistance_, *spinPixelSize_;

  // REVIEW
  void setTo(TheHub&);
  void setFrom(TheHub&);
};

class DatasetOptions2 : public BoxPanel {
  Q_OBJECT
  CLS(DatasetOptions2) SUPER(BoxPanel)
public:
  DatasetOptions2(TheHub&);

signals:
  void imageScale(preal);

private:
  QSpinBox  *marginLeft_, *marginTop_, *marginRight_, *marginBottom_;
  QSlider   *sliderImageScale_;
  QComboBox *comboNormType_;

  void setFrom(TheHub&);
};

//------------------------------------------------------------------------------
}}
#endif // PANEL_OPTIONS_H
