// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      typ_async.h
//! @brief     Asynchronous computation support.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef TYP_LOG_H
#define TYP_LOG_H

#include "def/defs.h"

class MessageLogger {
public:
  static void info(rcstr);
  static void warn(rcstr);

  static void (*handler)(rcstr);
};

#endif // TYP_LOG_H
