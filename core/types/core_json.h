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
#include <QJsonObject>

namespace core {
//------------------------------------------------------------------------------

class JsonObj: public QJsonObject {
  SUPER(JsonObj,QJsonObject)
public:
  JsonObj();
  JsonObj(QJsonObject const&);

  int      loadInt(rcstr key) const THROWS;
  JsonObj& saveInt(rcstr key, int);

  uint     loadUint(rcstr key) const THROWS;
  JsonObj& saveUint(rcstr key, uint);

  qreal    loadReal(rcstr key) const THROWS;
  JsonObj& saveReal(rcstr key, qreal);

  JsonObj& operator+= (JsonObj const&);
  JsonObj  operator+  (JsonObj const&) const;
};

//------------------------------------------------------------------------------
}
#endif // CORE_JSON_H
