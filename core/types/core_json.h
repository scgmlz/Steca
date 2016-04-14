// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_json.h
//! @brief     Json load / save helper.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
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
  JsonObj  loadObj(rcstr key) const THROWS;

  JsonObj&   saveArr(rcstr key, QJsonArray const&);
  QJsonArray loadArr(rcstr key) const THROWS;

  JsonObj& saveInt(rcstr key, int);
  int      loadInt(rcstr key) const THROWS;

  JsonObj& saveUint(rcstr key, uint);
  uint     loadUint(rcstr key) const THROWS;

  JsonObj& saveReal(rcstr key, qreal);
  qreal    loadReal(rcstr key) const THROWS;

  JsonObj& saveBool(rcstr key, bool);
  bool     loadBool(rcstr key) const THROWS;

  JsonObj& saveString(rcstr key, rcstr);
  str      loadString(rcstr key) const THROWS;

  JsonObj& saveRange(rcstr key, rcRange);
  Range    loadRange(rcstr key) const THROWS;

  JsonObj& saveXY(rcstr key, rcXY);
  XY       loadXY(rcstr key) const THROWS;

  JsonObj& saveIJ(rcstr key, rcIJ);
  IJ       loadIJ(rcstr key) const THROWS;

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
