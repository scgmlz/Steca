/** \file
 * Core-supporting types and functions.
 */

#include "core_defs.h"

#ifndef CORE_TYPES_H
#define CORE_TYPES_H

namespace core {

/// a range of values (a closed interval) // TODO make it right-open?
struct Range {
  Range();
  Range(qreal val);
  Range(qreal min, qreal max);

  static Range infinite();

  qreal min, max;

  void invalidate();              ///< make NaN
  bool isValid() const;

  void set(qreal val);            ///< both min and max = val
  void set(qreal min,qreal max);  ///< must be: min<=max
  void safeSet(qreal,qreal);      ///< will be set in the right order min/max
  static Range safeFrom(qreal,qreal);

  void extend(qreal val);         ///< extend to include val
  void extend(Range const&);      ///< extend to include the other range

  bool contains(qreal val)      const;
  bool contains(Range const&)   const;
  bool intersects(Range const&) const;
};

/// A set of *sorted* non-overlapping ranges
class Ranges {
public:
  Ranges();
  typedef QVector<Range> ranges_t;

  bool isEmpty() const;

  ranges_t const& getData() const {
    return ranges;
  }

  void clear();

  bool add(Range const&);
  bool rem(Range const&);

private:
  void sort();
  ranges_t ranges;
};

// TODO see if all three-times-two are needed, disassemble?
struct Borders {
  Range
    gamma,
    tth_regular,
    tth_gamma0; // at gamma=0

  void invalidate();
  bool isValid() const;
};

// cut
struct ImageCut {
  ImageCut(uint top = 0, uint bottom = 0, uint left = 0, uint right = 0);
  bool operator==(ImageCut const&);
  uint top, bottom, left, right;

  uint getWidth(QSize const&) const;
  uint getHeight(QSize const&) const;
  uint getCount(QSize const&) const;
};

// angle mapping
struct Pixpos {  // TODO bad name
  Pixpos(): Pixpos(0,0) {}
  Pixpos(qreal gamma, qreal tth): gammaPix(gamma), tthPix(tth) {}
  qreal gammaPix;
  qreal tthPix;
};

// conversion
qreal deg_rad(qreal rad);         ///< conversion: degrees <= radians
qreal rad_deg(qreal deg);         ///< conversion: radians <= degrees

// vectors
typedef QVector<qreal> reals_t;
typedef QVector<uint>  uints_t;

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

class TI_Curve: public Curve {
  SUPER(TI_Curve,Curve)
public:
  TI_Curve();

  void clear();

  void append(qreal tth,qreal inten);

  reals_t const& getTth()        const { return getXs(); }
  reals_t const& getInten()      const { return getYs(); }

  Range   const& getTthRange()   const { return tthRange;   }
  Range   const& getIntenRange() const { return intenRange; }

private:
  Range tthRange, intenRange;
};

}

#endif
