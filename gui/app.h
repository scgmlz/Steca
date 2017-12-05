// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/app.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //


#ifndef APP_H
#define APP_H

#include "def/defs.h"
#include <QApplication>


class NoWarnings {
public:
  NoWarnings();
 ~NoWarnings();
};

class App : public QApplication {
  CLASS(App) SUPER(QApplication)
public:
  App(int& argc, char* argv[]);

  int exec();

private:
  // Exceptions caught here; displayed in a dialog.
  bool notify(QObject*, QEvent*);
};


#endif // APP_H
