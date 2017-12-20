// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/json.cpp
//! @brief     Implements class JsonObj
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "typ/exception.h"
#include "typ/ij.h"
#include "typ/json.h"
#include "typ/range.h"
#include "typ/realpair.h"
#include "typ/str.h"
#include <QStringList> // needed under Travis

namespace typ {

JsonObj::JsonObj() {}

JsonObj::JsonObj(QJsonObject const& obj) : super(obj) {}

JsonObj& JsonObj::saveObj(rcstr key, JsonObj const& obj) {
    insert(key, obj.sup());
    return *this;
}

JsonObj JsonObj::loadObj(rcstr key, bool defEmpty) const THROWS {
    const QJsonValue& val = value(key);
    switch (val.type()) {
    case QJsonValue::Object:
        return val.toObject();
    case QJsonValue::Undefined:
        if (defEmpty)
            return {};
    default:
        THROW(key + ": not an object");
    }
}

JsonObj& JsonObj::saveArr(rcstr key, QJsonArray const& arr) {
    insert(key, arr);
    return *this;
}

QJsonArray JsonObj::loadArr(rcstr key, bool defEmpty) const THROWS {
    const QJsonValue& val = value(key);
    switch (val.type()) {
    case QJsonValue::Array:
        return val.toArray();
    case QJsonValue::Undefined:
        if (defEmpty)
            return {};
    default:
        THROW(key + ": not an array");
    }
}

JsonObj& JsonObj::saveInt(rcstr key, int num) {
    insert(key, num);
    return *this;
}

int JsonObj::loadInt(rcstr key) const THROWS {
    const QJsonValue& val = value(key);
    switch (val.type()) {
    case QJsonValue::Double: return qRound(val.toDouble());
    default: THROW(key + ": bad number format");
    }
}

int JsonObj::loadInt(rcstr key, int def) const THROWS{
    return value(key).isUndefined() ? def : loadInt(key);
}

JsonObj& JsonObj::saveUint(rcstr key, uint num) {
    return saveInt(key, to_i(num));
}

uint JsonObj::loadUint(rcstr key) const THROWS {
    int num = loadInt(key);
    if (num < 0)
        THROW(key + ": bad number format");
    return to_u(num);
}

uint JsonObj::loadUint(rcstr key, uint def) const THROWS{
    return value(key).isUndefined() ? def : loadUint(key);
}

JsonObj& JsonObj::savePint(rcstr key, pint num) {
    return saveUint(key, num);
}

pint JsonObj::loadPint(rcstr key) const {
    uint num = loadUint(key);
    RUNTIME_CHECK(num > 0, "expecting positive number");
    return pint(num);
}

pint JsonObj::loadPint(rcstr key, uint def) const {
    return value(key).isUndefined() ? (pint)def : loadPint(key);
}

JsonObj& JsonObj::saveQreal(rcstr key, qreal num) {
    if (qIsNaN(num)) {
        // do not save anything for NaNs
    } else if (qIsInf(num)) {
        insert(key, num < 0 ? "-inf" : "+inf");
    } else {
        insert(key, num);
    }
    return *this;
}

qreal JsonObj::loadQreal(rcstr key) const THROWS {
    const QJsonValue& val = value(key);

    switch (val.type()) {
    case QJsonValue::Undefined:
        return NAN; // not present means not a number
    case QJsonValue::String: { // infinities stored as strings
        const str& s = val.toString();
        if (s == "+inf")
            return +INF;
        if (s == "-inf")
            return -INF;
        THROW(key + ": bad number format");
    }
    default: return val.toDouble();
    }
}

qreal JsonObj::loadQreal(rcstr key, qreal def) const THROWS{
    return value(key).isUndefined() ? def : loadQreal(key);
}

JsonObj& JsonObj::savePreal(rcstr key, preal num) {
    return saveQreal(key, num);
}

preal JsonObj::loadPreal(rcstr key) const {
    qreal num = loadQreal(key);
    RUNTIME_CHECK(num >= 0, "expecting positive number");
    return preal(num);
}

preal JsonObj::loadPreal(rcstr key, preal def) const {
    return value(key).isUndefined() ? def : loadPreal(key);
}

bool JsonObj::loadBool(rcstr key) const THROWS {
    const QJsonValue& val = value(key);
    switch (val.type()) {
    case QJsonValue::Bool: return val.toBool();
    default: THROW(key + ": not a boolean");
    }
}

bool JsonObj::loadBool(rcstr key, bool def) const THROWS{
    return value(key).isUndefined() ? def : loadBool(key);
}

str JsonObj::loadString(rcstr key) const THROWS {
    const QJsonValue& val = value(key);
    switch (val.type()) {
    case QJsonValue::String: return val.toString();
    default: THROW(key + ": not a string");
    }
}

str JsonObj::loadString(rcstr key, rcstr def) const THROWS{
    return value(key).isUndefined() ? def : loadString(key);
}

Range JsonObj::loadRange(rcstr key) const THROWS {
    Range range;
    range.from_json(loadObj(key));
    return range;
}

IJ JsonObj::loadIJ(rcstr key) const THROWS {
    IJ ij;
    ij.from_json(loadObj(key));
    return ij;
}

JsonObj& JsonObj::saveqpair(rcstr key, qpair const& qpair) {
    insert(key, qpair.to_json());
    return *this;
}

qpair JsonObj::loadqpair(rcstr key) const THROWS {
    qpair qpair;
    qpair.from_json(loadObj(key));
    return qpair;
}

} // namespace typ
