// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_type_range.h
//! @brief     Range of real values
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_TYPE_RANGE_H
#define CORE_TYPE_RANGE_H

#include "core_defs.h"
#include "core_types_fwd.h"

namespace core {
//------------------------------------------------------------------------------
/// a range of values - a closed interval

struct Range {
  Range();                        ///< invalid (NaN)
  Range(qreal val);               ///< singular, min == max
  Range(qreal min, qreal max);    ///< normal

  static Range infinite();        ///< factory: -inf .. +inf

  void  invalidate();             ///< make invalid
  bool  isValid() const;          ///< is not NaN

  qreal width()  const;
  qreal center() const;

  qreal min, max;                 ///< this is the range

  void  set(qreal val);           ///< make singular
  void  set(qreal min,qreal max); ///< must be: min <= max
  void  safeSet(qreal,qreal);     ///< will be set in the right order min/max

  static Range safeFrom(qreal,qreal); ///< safe factory

  void  extendBy(qreal);          ///< extend to include the number
  void  extendBy(rcRange);   ///< extend to include the range

  // these may be called only on valid ranges
  bool  contains(qreal val)      const;
  bool  contains(rcRange)   const;
  bool  intersects(rcRange) const;

  /// limit the number to the interval, as qBound would
  qreal bound(qreal)             const;

  JsonObj saveJson() const;
  void    loadJson(rcJsonObj) THROWS;
};

//------------------------------------------------------------------------------
/// A set of *sorted* non-overlapping ranges

class Ranges {
public:
  Ranges();

  void clear()             { _ranges.clear();           }

  bool isEmpty()     const { return _ranges.isEmpty();  }
  uint count()       const { return _ranges.count();    }

  rcRange at(uint i) const { return _ranges.at(i);      }

  /// collapses overlapping ranges into one; returns whether there was a change
  bool add(rcRange);
  /// removes (cuts out) a range; returns whether there was a change
  bool rem(rcRange);

private:
  void sort();
  QVector<Range> _ranges;

public:
  JsonArr saveJson() const;
  void    loadJson(rcJsonArr) THROWS;
};

//------------------------------------------------------------------------------
}
#endif // CORE_TYPE_RANGE_H
