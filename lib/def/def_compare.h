// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      def_macros.h
//! @brief     Helper macros for defining types end else.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle, Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef DEF_COMPARE_H
#define DEF_COMPARE_H

// a helper for int compare()
#define COMPARE_VALUE(val)        \
  if (val < that.val) return -1;  \
  if (val > that.val) return +1;

#define COMPARE_VALUE2(v1,v2)     \
  if (v1 < v2) return -1;         \
  if (v1 > v2) return +1;

#define COMPARE_COMPARABLE(o) \
  for (int cmp = o.compare(that.o); cmp; ) \
    return cmp;

#endif // DEF_COMPARE_H
