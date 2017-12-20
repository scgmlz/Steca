// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/json.h
//! @brief     Defines class JsonObj
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef JSON_H
#define JSON_H

#include "def/numbers.h"
#include "typ/exception.h"
#include "typ/str.h"
#include <QJsonArray>
#include <QJsonObject>

class qpair;

namespace typ {

struct Range;
struct IJ;

class JsonObj : public QJsonObject {
private:
    using super = QJsonObject;
public:
    JsonObj();
    JsonObj(QJsonObject const&);

    super const& sup() const { return *this; }

    JsonObj& saveObj(rcstr key, JsonObj const&);
    JsonObj loadObj(rcstr key, bool defEmpty = false) const THROWS;

    JsonObj& saveArr(rcstr key, QJsonArray const&);
    QJsonArray loadArr(rcstr key, bool defEmpty = false) const THROWS;

    JsonObj& saveInt(rcstr key, int);
    int loadInt(rcstr key) const THROWS;
    int loadInt(rcstr key, int def) const THROWS;

    JsonObj& saveUint(rcstr key, uint);
    uint loadUint(rcstr key) const THROWS;
    uint loadUint(rcstr key, uint def) const THROWS;

    JsonObj& savePint(rcstr key, pint);
    pint loadPint(rcstr key) const THROWS;
    pint loadPint(rcstr key, uint def) const THROWS;

    JsonObj& saveQreal(rcstr key, qreal);
    qreal loadQreal(rcstr key) const THROWS;
    qreal loadQreal(rcstr key, qreal def) const THROWS;

    JsonObj& savePreal(rcstr key, preal);
    preal loadPreal(rcstr key) const THROWS;
    preal loadPreal(rcstr key, preal def) const THROWS;

    JsonObj& saveBool(rcstr key, bool);
    bool loadBool(rcstr key) const THROWS;
    bool loadBool(rcstr key, bool def) const THROWS;

    JsonObj& saveString(rcstr key, rcstr);
    str loadString(rcstr key) const THROWS;
    str loadString(rcstr key, rcstr def) const THROWS;

    Range loadRange(rcstr key) const THROWS;

    JsonObj& saveIJ(rcstr key, IJ const&);
    IJ loadIJ(rcstr key) const THROWS;

    JsonObj& saveqpair(rcstr key, qpair const&);
    qpair loadqpair(rcstr key) const THROWS;

    JsonObj& operator+=(JsonObj const&);
    JsonObj operator+(JsonObj const&) const;
};

} // namespace typ

#endif // JSON_H
