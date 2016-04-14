// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_json.h
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_json.h"
#include "core_type_range.h"
#include "core_coords.h"
#include <QStringList>

namespace core {
//------------------------------------------------------------------------------

JsonObj::JsonObj() {
}

JsonObj::JsonObj(QJsonObject const& obj): super(obj) {
}

JsonObj& JsonObj::saveObj(rcstr key, QJsonObject const& obj) {
  insert(key, obj);
  return *this;
}

JsonObj JsonObj::loadObj(rcstr key) const THROWS {
  auto val = value(key);

  switch (val.type()) {
  case QJsonValue::Object:
    return val.toObject();
  default:
    THROW(key + ": not an object");
  }
}

JsonObj& JsonObj::saveArr(rcstr key, QJsonArray const& arr) {
  insert(key, arr);
  return *this;
}

QJsonArray JsonObj::loadArr(rcstr key) const THROWS {
  auto val = value(key);

  switch (val.type()) {
  case QJsonValue::Array:
    return val.toArray();
  default:
    THROW(key + ": not an array");
  }
}

JsonObj& JsonObj::saveInt(rcstr key, int num) {
  insert(key, num);
  return *this;
}

int JsonObj::loadInt(rcstr key) const THROWS {
  auto val = value(key);

  switch (val.type()) {
  case QJsonValue::Double:
    return qRound(val.toDouble());
  default:
    THROW(key + ": bad number format");
  }
}

JsonObj& JsonObj::saveUint(rcstr key, uint num) {
  return saveInt(key,num);
}

uint JsonObj::loadUint(rcstr key) const THROWS {
  int num = loadInt(key);
  if (num < 0) THROW(key + ": bad number format");
  return (uint)num;
}

static str const INF_P("+inf"), INF_M("-inf");

JsonObj& JsonObj::saveReal(rcstr key, qreal num) {
  if (qIsNaN(num)) {
    // do not save anything for NaNs
  } else if (qIsInf(num)) {
    insert(key, num < 0 ? INF_M : INF_P);
  } else {
    insert(key, num);
  }

  return *this;
}

qreal JsonObj::loadReal(rcstr key) const THROWS {
  auto val = value(key);

  switch (val.type()) {
  case QJsonValue::Undefined:
    return qQNaN();             // not present means not a number
  case QJsonValue::String: {    // infinities stored as strings
    auto s = val.toString();
    if (INF_P == s) return +qInf();
    if (INF_M == s) return -qInf();
    THROW(key + ": bad number format");
  }
  default:
    return val.toDouble();
  }
}

JsonObj& JsonObj::saveBool(rcstr key, bool b) {
  insert(key, b);
  return *this;
}

bool JsonObj::loadBool(rcstr key) const THROWS {
  auto val = value(key);

  switch (val.type()) {
  case QJsonValue::Bool:
    return val.toBool();
  default:
    THROW(key + ": not a boolean");
  }
}

JsonObj& JsonObj::saveString(rcstr key, rcstr s) {
  insert(key, s);
  return *this;
}

str JsonObj::loadString(rcstr key) const THROWS {
  auto val = value(key);

  switch (val.type()) {
  case QJsonValue::String:
    return val.toString();
  default:
    THROW(key + ": not a string");
  }
}

JsonObj& JsonObj::saveRange(rcstr key, rcRange range) {
  insert(key, range.saveJson());
  return *this;
}

Range JsonObj::loadRange(rcstr key) const THROWS {
  Range range; range.loadJson(loadObj(key));
  return range;
}

JsonObj& JsonObj::saveXY(rcstr key, rcXY xy) {
  insert(key, xy.saveJson());
  return *this;
}

XY JsonObj::loadXY(rcstr key) const THROWS {
  XY xy; xy.loadJson(loadObj(key));
  return xy;
}

JsonObj& JsonObj::saveIJ(rcstr key, rcIJ ij) {
  insert(key, ij.saveJson());
  return *this;
}

IJ JsonObj::loadIJ(rcstr key) const THROWS {
  IJ ij; ij.loadJson(loadObj(key));
  return ij;
}

JsonObj& JsonObj::operator+= (rcJsonObj that) {
  for (auto &key: that.keys())
    insert(key,that[key]);
  return *this;
}

JsonObj JsonObj::operator+ (rcJsonObj that) const {
  return JsonObj(*this) += that;
}

//------------------------------------------------------------------------------

JsonArr::JsonArr() {
}

JsonArr::JsonArr(QJsonArray const& array): super(array) {
}

JsonObj JsonArr::objAt(uint i) const {
  auto obj = super::at(i);
  RUNTIME_CHECK(QJsonValue::Object == obj.type(), "not an object at " + str::number(i));
  return super::at(i).toObject();
}

//------------------------------------------------------------------------------
}
// eof
