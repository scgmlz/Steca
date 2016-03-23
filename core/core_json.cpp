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
#include <QStringList>

namespace core {
//------------------------------------------------------------------------------

JsonObj::JsonObj() {
}

JsonObj::JsonObj(QJsonObject const& obj): super(obj) {
}

static str const INF_P("+inf"), INF_M("-inf");

qreal JsonObj::loadReal(rcstr key) const THROWS {
  auto val = value(key);

  switch (val.type()) {
  case QJsonValue::Undefined:
    return qQNaN();             // not present means not a number
  case QJsonValue::String: {    // infinities stored as strings
    auto s = val.toString();
    if (INF_P == s) return +qInf();
    if (INF_M == s) return -qInf();
    THROW("bad number format");
  }
  default:
    return val.toDouble();
  }
}

void JsonObj::saveReal(rcstr key, qreal num) {
  if (qIsNaN(num)) return;      // do not save anything

  if (qIsInf(num))
    insert(key, num < 0 ? INF_M : INF_P);
  else
    insert(key, num);
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
