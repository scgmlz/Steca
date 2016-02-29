/** \file
 * Common core types.
 */

#include "core_defs.h"

#ifndef CORE_TYPES_H
#define CORE_TYPES_H

namespace core {
//------------------------------------------------------------------------------
// conversions

qreal deg_rad(qreal rad);         ///< conversion: degrees <= radians
qreal rad_deg(qreal deg);         ///< conversion: radians <= degrees

//------------------------------------------------------------------------------
/// a range of values (a closed interval)

struct Range {
  Range();
  Range(qreal val);
  Range(qreal min, qreal max);

  static Range infinite();        ///< -inf .. +inf

  qreal min, max;                 ///< this is the range

  void invalidate();              ///< make NaN
  bool isValid() const;           ///< not NaN

  void set(qreal val);            ///< both min and max = val
  void set(qreal min,qreal max);  ///< must be: min <= max
  void safeSet(qreal,qreal);      ///< will be set in the right order min-max
  static Range safeFrom(qreal,qreal);

  void extend(qreal val);         ///< extend to include val
  void extend(Range const&);      ///< extend to include the other range

  bool contains(qreal val)      const;
  bool contains(Range const&)   const;
  bool intersects(Range const&) const;
};

//------------------------------------------------------------------------------
/// A set of *sorted* non-overlapping ranges

class Ranges {
public:
  Ranges();

  bool isEmpty()          const { return ranges.isEmpty(); }
  uint count()            const { return ranges.count();   }
  Range const& at(uint i) const { return ranges.at(i);     }

  void clear();
  bool add(Range const&);
  bool rem(Range const&);

private:
  void sort();
  QVector<Range> ranges;
};

//------------------------------------------------------------------------------
/// A set of datapoints.

class Curve {
public:
  Curve();
  virtual ~Curve();

  virtual void clear();

  bool isEmpty() const;
  uint count()   const;
  bool isOrdered() const;

  virtual void append(qreal x, qreal y);

  reals_t const& getXs() const { return xs; }
  reals_t const& getYs() const { return ys; }

private:
  reals_t xs, ys;
};

//------------------------------------------------------------------------------
/// 2theta -> intensity curve

class TI_Curve: public Curve {
  SUPER(TI_Curve,Curve)
public:
  TI_Curve();

  void clear();
  void append(qreal tth,qreal inten);

  reals_t const& getTth()             const { return getXs();     }
  reals_t const& getInten()           const { return getYs();     }

  core::Range const& getTthRange()    const { return tthRange;    }
  core::Range const& getIntenRange()  const { return intenRange;  }

private:
  core::Range tthRange, intenRange;
};

//------------------------------------------------------------------------------
}
#endif
