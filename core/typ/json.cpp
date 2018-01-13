// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/json.cpp
//! @brief     Implements function qreal_to_json and class JsonObj
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "core/typ/ij.h"
#include "core/typ/json.h"
#include "core/typ/range.h"
#include <QStringList> // needed under Travis

// ************************************************************************** //
//  To support Json output
// ************************************************************************** //

QJsonValue qreal_to_json(const qreal num) {
    if (qIsNaN(num))
        return "nan";
    else if (qIsInf(num))
        return num < 0 ? "-inf" : "+inf";
    else
        return num;
}


// ************************************************************************** //
//  To read Json input
// ************************************************************************** //

JsonObj::JsonObj() {}

JsonObj::JsonObj(QJsonObject const& obj) : super(obj) {}

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

int JsonObj::loadUint(rcstr key) const THROWS {
    int num = loadInt(key);
    if (num < 0)
        THROW(key + ": bad number format");
    return num;
}

int JsonObj::loadUint(rcstr key, int def) const THROWS{
    return value(key).isUndefined() ? def : loadUint(key);
}

int JsonObj::loadPint(rcstr key) const {
    int num = loadUint(key);
    RUNTIME_CHECK(num > 0, "expecting positive number");
    return num;
}

int JsonObj::loadPint(rcstr key, int def) const {
    return value(key).isUndefined() ? def : loadPint(key);
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
        if (s == "nan")
            return NAN;
        THROW(key + ": bad number format");
    }
    default: return val.toDouble();
    }
}

qreal JsonObj::loadQreal(rcstr key, qreal def) const THROWS{
    return value(key).isUndefined() ? def : loadQreal(key);
}

preal JsonObj::loadPreal(rcstr key) const {
    qreal num = loadQreal(key);
    RUNTIME_CHECK(num > 0, "expecting positive number");
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
