// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      app.h
//! @brief     The application.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Antti Soininen, Jan Burle, Rebecca Brydon
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef APP_H
#define APP_H

#include "types/core_defs.h"
#include <QApplication>

//------------------------------------------------------------------------------

class App : public QApplication {
  SUPER(App, QApplication)
public:
  App(int& argc, char* argv[]);

  int exec();

private:
  /// Exceptions caught here; displayed in a dialog.
  bool notify(QObject*, QEvent*);
};

//------------------------------------------------------------------------------
#endif  // APP_H
