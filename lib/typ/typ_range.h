/*******************************************************************************
 * STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#ifndef TYP_RANGE_H
#define TYP_RANGE_H

#include "def/def_macros.h"
#include "typ/typ_vec.h"

namespace typ {
//------------------------------------------------------------------------------
// a range of values - a closed interval

class JsonObj;
class JsonArr;

struct Range {
  CLS(Range)

  Range();                          // invalid (NaN)
  Range(qreal val);                 // singular, min == max
  Range(qreal min, qreal max);      // normal

  static Range infinite();          // factory: -inf .. +inf

  void  invalidate();               // make invalid
  bool  isValid() const;            // is not NaN
  bool  isEmpty() const;            // not valid or empty

  qreal width()  const;
  qreal center() const;

  qreal min, max;                   // this is the range

  void  set(qreal val);             // make singular
  void  set(qreal min, qreal max);  // must be: min <= max
  void  safeSet(qreal, qreal);      // will be set in the right order min/max

  static Range safeFrom(qreal, qreal); // safe factory

  void  extendBy(qreal);            // extend to include the number
  void  extendBy(rc);               // extend to include the range

  bool  contains(qreal) const;
  bool  contains(rc)    const;
  bool  intersects(rc)  const;
  Range intersect(rc)   const;

  // limit the number to the interval, as qBound would
  qreal bound(qreal)    const;

  JsonObj saveJson()    const;
  void loadJson(JsonObj const&) THROWS;
};

#ifndef QT_NO_DEBUG
QDebug& operator<<(QDebug&, Range::rc);
#endif

//------------------------------------------------------------------------------
// A set of *sorted* *non-overlapping* ranges

class Ranges {
  CLS(Ranges)
public:
  Ranges();

  void clear()               { ranges_.clear();           }
  bool isEmpty()       const { return ranges_.isEmpty();  }
  uint count()         const { return ranges_.count();    }

  Range::rc at(uint i) const { return ranges_.at(i);      }

  // collapses overlapping ranges into one; returns whether there was a change
  bool add(Range::rc);

  // removes (cuts out) a range; returns whether there was a change
  bool rem(Range::rc);

private:
  void sort();
  vec<Range> ranges_;

public:
  JsonArr saveJson() const;
  void loadJson(JsonArr const&) THROWS;
};

//------------------------------------------------------------------------------
}
#endif // TYP_RANGE_H
