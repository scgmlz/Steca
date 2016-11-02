// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      typ_async.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle, Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "typ_log.h"

//------------------------------------------------------------------------------

void MessageLogger::info(rcstr msg) {
  if (handler)
    handler(msg, INFO);
}

void MessageLogger::warn(rcstr msg) {
  if (handler)
    handler(msg, WARN);
}

void MessageLogger::popup(rcstr msg) {
  if (handler)
    handler(msg, POPUP);
}

void (*MessageLogger::handler)(rcstr, eType) = nullptr;

//------------------------------------------------------------------------------
// eof
