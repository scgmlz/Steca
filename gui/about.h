// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      about.h
//! @brief     The about dialog & configuration
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle, Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef ABOUT_H
#define ABOUT_H

#include "thehub.h"
#include "def/defs.h"

#include <QDialog>

namespace gui {
//------------------------------------------------------------------------------

class AboutBox : public QDialog {
  CLS(AboutBox) SUPER(QDialog)
public:
  AboutBox(QWidget*);

protected:
  void accept();
  void mouseDoubleClickEvent(QMouseEvent*);

  QCheckBox      *cbShowAtStartup_, *cbCheckUpdatesAtStartup_;
  QDoubleSpinBox *detDistance_, *detPixelSize_;
};

//------------------------------------------------------------------------------
}
#endif // MAINWIN_H
