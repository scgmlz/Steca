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
  void  extendBy(Range const&);   ///< extend to include the range

  // these may be called only on valid ranges
  bool  contains(qreal val)      const;
  bool  contains(Range const&)   const;
  bool  intersects(Range const&) const;

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

  void clear()                  { ranges.clear();           }

  bool isEmpty()          const { return ranges.isEmpty();  }
  uint count()            const { return ranges.count();    }

  Range const& at(uint i) const { return ranges.at(i);      }

  /// collapses overlapping ranges into one; returns whether there was a change
  bool add(Range const&);
  /// removes (cuts out) a range; returns whether there was a change
  bool rem(Range const&);

private:
  void sort();
  QVector<Range> ranges;

public:
  JsonObj saveJson() const;
  void    loadJson(rcJsonObj) THROWS;
};

//------------------------------------------------------------------------------
}
#endif // CORE_TYPE_RANGE_H
