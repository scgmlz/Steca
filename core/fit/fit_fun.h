// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      fit_fun.h
//! @brief     Functions for data fitting
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef FIT_FUN_H
#define FIT_FUN_H

#include "typ/typ_fun.h"
#include "typ/typ_curve.h"
#include "typ/typ_xy.h"

namespace fit {
//------------------------------------------------------------------------------
// a polynom(ial)

class Polynom : public typ::SimpleFunction {
  SUPER(Polynom, typ::SimpleFunction)
public:
  Polynom(uint degree = 0);

  uint degree() const;
  void setDegree(uint);

  qreal y(qreal x, qreal const* parValues = nullptr) const;
  qreal dy(qreal x, uint parIndex, qreal const* parValues = nullptr) const;

  qreal avgY(typ::Range::rc, qreal const* parValues = nullptr) const;

  void fit(typ::Curve::rc, typ::Ranges::rc);
  static Polynom fromFit(uint degree, typ::Curve::rc, typ::Ranges::rc);

public:
  typ::JsonObj saveJson() const;
  void loadJson(typ::JsonObj::rc) THROWS;
};

//------------------------------------------------------------------------------
// Abstract peak function

enum class ePeakType {
  RAW, GAUSSIAN, LORENTZIAN, PSEUDOVOIGT1, PSEUDOVOIGT2,
  NUM_TYPES
};

class PeakFunction : public typ::SimpleFunction {
  SUPER(PeakFunction, typ::SimpleFunction)
public:

  static PeakFunction* factory(ePeakType);

  PeakFunction();
  PeakFunction* clone() const;

  virtual ePeakType type() const = 0;

  typ::Range::rc range() const { return range_; }
  virtual void setRange(typ::Range::rc);

  virtual void setGuessedPeak(typ::XY::rc);
  virtual void setGuessedFWHM(qreal);

  typ::XY::rc guessedPeak() const { return guessedPeak_; }
  qreal       guessedFWHM() const { return guessedFWHM_; }

  virtual typ::XY fittedPeak() const = 0;
  virtual qreal   fittedFWHM() const = 0;

  virtual typ::XY peakError() const = 0;
  virtual qreal   fwhmError() const = 0;

  void reset();

  void fit(typ::Curve::rc curve ) { return fit(curve, range_); }

  virtual void fit(typ::Curve::rc, typ::Range::rc);

protected:
  typ::Curve prepareFit(typ::Curve::rc, typ::Range::rc);

public:
  typ::JsonObj saveJson() const;
  void loadJson(typ::JsonObj::rc) THROWS;

protected:
  typ::Range range_;
  typ::XY    guessedPeak_;
  qreal      guessedFWHM_;
};

//------------------------------------------------------------------------------

class Raw : public PeakFunction {
  SUPER(Raw, PeakFunction)
public:
  Raw();

  ePeakType type() const { return ePeakType::RAW; }

  qreal y(qreal x, qreal const* parValues = nullptr) const;
  qreal dy(qreal x, uint parIndex, qreal const* parValues = nullptr) const;

  typ::XY fittedPeak() const;
  qreal   fittedFWHM() const;

  typ::XY peakError() const;
  qreal   fwhmError() const;

  void setRange(typ::Range::rc);
  void fit(typ::Curve::rc, typ::Range::rc);

private:
  typ::Curve fittedCurve_;  // saved from fitting
  void  prepareY();

  mutable uint  x_count_;
  mutable qreal dx_;
  mutable qreal sum_y_;

public:
  typ::JsonObj saveJson() const;
};

//------------------------------------------------------------------------------

class Gaussian : public PeakFunction {
  SUPER(Gaussian, PeakFunction)
public:
  enum { parAMPL, parXSHIFT, parSIGMA };

  Gaussian(qreal ampl = 1, qreal xShift = 0, qreal sigma = 1);

  ePeakType type() const { return ePeakType::GAUSSIAN; }

  qreal y(qreal x, qreal const* parValues = nullptr) const;
  qreal dy(qreal x, uint parIndex, qreal const* parValues = nullptr) const;

  void setGuessedPeak(typ::XY::rc);
  void setGuessedFWHM(qreal);

  typ::XY fittedPeak() const;
  qreal   fittedFWHM() const;

  typ::XY peakError() const;
  qreal   fwhmError() const;

public:
  typ::JsonObj saveJson() const;
};

//------------------------------------------------------------------------------

class Lorentzian : public PeakFunction {
  SUPER(Lorentzian, PeakFunction)
public:
  enum { parAMPL, parXSHIFT, parGAMMA };

  Lorentzian(qreal ampl = 1, qreal xShift = 0, qreal gamma = 1);

  ePeakType type() const { return ePeakType::LORENTZIAN; }

  qreal y(qreal x, qreal const* parValues = nullptr) const;
  qreal dy(qreal x, uint parIndex, qreal const* parValues = nullptr) const;

  void setGuessedPeak(typ::XY::rc);
  void setGuessedFWHM(qreal);

  typ::XY fittedPeak() const;
  qreal   fittedFWHM() const;

  typ::XY peakError() const;
  qreal   fwhmError() const;

public:
  typ::JsonObj saveJson() const;
};

//------------------------------------------------------------------------------

class PseudoVoigt1 : public PeakFunction {
  SUPER(PseudoVoigt1, PeakFunction)
public:
  enum { parAMPL, parXSHIFT, parSIGMAGAMMA, parETA };

  PseudoVoigt1(qreal ampl = 1, qreal xShift = 0, qreal sigmaGamma = 1,
               qreal eta = 0.1);

  ePeakType type() const { return ePeakType::PSEUDOVOIGT1; }

  qreal y(qreal x, qreal const* parValues = nullptr) const;
  qreal dy(qreal x, uint parIndex, qreal const* parValues = nullptr) const;

  void setGuessedPeak(typ::XY::rc);
  void setGuessedFWHM(qreal);

  typ::XY fittedPeak() const;
  qreal   fittedFWHM() const;

  typ::XY peakError() const;
  qreal   fwhmError() const;

public:
  typ::JsonObj saveJson() const;
};

//------------------------------------------------------------------------------

class PseudoVoigt2 : public PeakFunction {
  SUPER(PseudoVoigt2, PeakFunction)
public:
  enum { parAMPL, parXSHIFT, parSIGMA, parGAMMA, parETA };

  PseudoVoigt2(qreal ampl = 1, qreal xShift = 0, qreal sigma = 1,
               qreal gamma = 1, qreal eta = 0.1);

  ePeakType type() const { return ePeakType::PSEUDOVOIGT2; }

  qreal y(qreal x, qreal const* parValues = nullptr) const;
  qreal dy(qreal x, uint parIndex, qreal const* parValues = nullptr) const;

  void setGuessedPeak(typ::XY::rc);
  void setGuessedFWHM(qreal);

  typ::XY fittedPeak() const;
  qreal   fittedFWHM() const;

  typ::XY peakError() const;
  qreal  fwhmError() const;

public:
  typ::JsonObj saveJson() const;
};

//------------------------------------------------------------------------------
}
#endif // FIT_FUN_H
