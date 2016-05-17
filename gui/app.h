// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      app.h
//! @brief     The application.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
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
