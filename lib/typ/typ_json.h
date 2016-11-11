// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      typ_json.h
//! @brief     Json load / save helper.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle, Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef TYP_JSON_H
#define TYP_JSON_H

#include "def/def_macros.h"
#include "def/def_gsl.h"
#include "typ/typ_str.h"
#include <QJsonArray>
#include <QJsonObject>

namespace json_key {
extern str const
  I, J, X, Y, MIN, MAX,
  PARAMS, TYPE, FUN,
  VALUE, RANGE, COUNT,
  PEAK, FWHM;
}

namespace typ {
//------------------------------------------------------------------------------

class JsonArr;
struct Range;
struct IJ;
struct XY;

class JsonObj : protected QJsonObject {
  CLS(JsonObj) WITH_SUPER(QJsonObject)
public:
  JsonObj();
  JsonObj(QJsonObject const&);

  JsonObj& saveObj(rcstr key, JsonObj const&);
  JsonObj  loadObj(rcstr key, bool defEmpty=false) const THROWS;

  JsonObj& saveArr(rcstr key, JsonArr const&);
  JsonArr  loadArr(rcstr key, bool defEmpty=false) const THROWS;

  JsonObj& saveInt(rcstr key, int);
  int      loadInt(rcstr key)               const THROWS;
  int      loadInt(rcstr key, int def)      const THROWS;

  JsonObj& saveUint(rcstr key, uint);
  uint     loadUint(rcstr key)              const THROWS;
  uint     loadUint(rcstr key, uint def)    const THROWS;

  JsonObj& savePint(rcstr key, pint);
  pint     loadPint(rcstr key)              const THROWS;
  pint     loadPint(rcstr key, uint def)    const THROWS;

  JsonObj& saveQreal(rcstr key, qreal);
  qreal    loadQreal(rcstr key)             const THROWS;
  qreal    loadQreal(rcstr key, qreal def)  const THROWS;

  JsonObj& savePreal(rcstr key, preal);
  preal    loadPreal(rcstr key)             const THROWS;
  preal    loadPreal(rcstr key, preal def)  const THROWS;

  JsonObj& saveBool(rcstr key, bool);
  bool     loadBool(rcstr key)              const THROWS;
  bool     loadBool(rcstr key, bool def)    const THROWS;

  JsonObj& saveString(rcstr key, rcstr);
  str      loadString(rcstr key)            const THROWS;
  str      loadString(rcstr key, rcstr def) const THROWS;

  JsonObj& saveRange(rcstr key, Range const&);
  Range    loadRange(rcstr key)             const THROWS;

  JsonObj& saveIJ(rcstr key, IJ const&);
  IJ       loadIJ(rcstr key)                const THROWS;

  JsonObj& saveXY(rcstr key, XY const&);
  XY       loadXY(rcstr key)                const THROWS;

  JsonObj& operator+= (rc);
  JsonObj  operator+  (rc) const;
};

//------------------------------------------------------------------------------

class JsonArr: protected QJsonArray {
  CLS(JsonArr) WITH_SUPER(QJsonArray)
public:
  JsonArr();
  JsonArr(QJsonArray const&);

  using super::append;
  void append(JsonObj::rc);

  using super::begin;
  using super::end;
  using super::constBegin;
  using super::constEnd;

  uint count() const;
  JsonObj objAt(uint) const;
};

//------------------------------------------------------------------------------
}
#endif // TYP_JSON_H
