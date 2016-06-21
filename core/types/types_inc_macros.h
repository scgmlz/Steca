// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      types_inc_macros.h
//! @brief     Helper macros for defining types.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef TYPES_INC_MACROS_H
#define TYPES_INC_MACROS_H

#include <QtGlobal>

/// A class definition helper that defines aliases for this and super class.
#define SUPER(cls, sup)        \
private:                       \
  using thisClass = cls;       \
  using super = sup;           \
public:                        \
  typedef thisClass const& rc; \
private:

#endif // TYPES_INC_MACROS_H
