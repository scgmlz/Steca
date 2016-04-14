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

JsonObj& JsonObj::saveRange(rcstr key, Range const& range) {
  insert(key, range.saveJson());
  return *this;
}

Range JsonObj::loadRange(rcstr key) const THROWS {
  Range range; range.loadJson(value(key).toObject());
  return range;
}

JsonObj& JsonObj::operator+= (JsonObj const& that) {
  for (auto &key: that.keys())
    insert(key,that[key]);
  return *this;
}

JsonObj JsonObj::operator+ (JsonObj const& that) const {
  return JsonObj(*this) += that;
}

//------------------------------------------------------------------------------
}
// eof
