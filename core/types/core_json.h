// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_json.h
//! @brief     Json load / save helper.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Antti Soininen, Jan Burle, Rebecca Brydon
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef CORE_JSON_H
#define CORE_JSON_H

#include "core_defs.h"
#include "core_types_fwd.h"
#include <QJsonObject>
#include <QJsonArray>

namespace core {
//------------------------------------------------------------------------------

class JsonObj: public QJsonObject {
  SUPER(JsonObj,QJsonObject)
public:
  JsonObj();
  JsonObj(QJsonObject const&);

  JsonObj& saveObj(rcstr key, QJsonObject const&);
  JsonObj  loadObj(rcstr key, bool defEmpty=false) const THROWS;

  JsonObj& saveArr(rcstr key, QJsonArray const&);
  JsonArr  loadArr(rcstr key, bool defEmpty=false) const THROWS;

  JsonObj& saveInt(rcstr key, int);
  int      loadInt(rcstr key)               const THROWS;
  int      loadInt(rcstr key, int def)      const THROWS;

  JsonObj& saveUint(rcstr key, uint);
  uint     loadUint(rcstr key)              const THROWS;
  uint     loadUint(rcstr key, uint def)    const THROWS;

  JsonObj& saveReal(rcstr key, qreal);
  qreal    loadReal(rcstr key)              const THROWS;
  qreal    loadReal(rcstr key, qreal def)   const THROWS;

  JsonObj& saveBool(rcstr key, bool);
  bool     loadBool(rcstr key)              const THROWS;
  bool     loadBool(rcstr key, bool def)    const THROWS;

  JsonObj& saveString(rcstr key, rcstr);
  str      loadString(rcstr key)            const THROWS;
  str      loadString(rcstr key, rcstr def) const THROWS;

  JsonObj& saveRange(rcstr key, rcRange);
  Range    loadRange(rcstr key)             const THROWS;

  JsonObj& saveXY(rcstr key, rcXY);
  XY       loadXY(rcstr key)                const THROWS;

  JsonObj& saveIJ(rcstr key, rcIJ);
  IJ       loadIJ(rcstr key)                const THROWS;

  JsonObj& operator+= (rcJsonObj);
  JsonObj  operator+  (rcJsonObj) const;
};

//------------------------------------------------------------------------------

class JsonArr: public QJsonArray {
  SUPER(JsonArr,QJsonArray)
public:
  JsonArr();
  JsonArr(QJsonArray const&);

  JsonObj objAt(uint) const;
};

//------------------------------------------------------------------------------
}
#endif // CORE_JSON_H
