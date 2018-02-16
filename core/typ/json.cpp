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

JsonObj::JsonObj(const QJsonObject& obj) : super(obj) {}

JsonObj JsonObj::loadObj(const QString& key, bool defEmpty) const THROWS {
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

QJsonArray JsonObj::loadArr(const QString& key, bool defEmpty) const THROWS {
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

int JsonObj::loadInt(const QString& key) const THROWS {
    const QJsonValue& val = value(key);
    switch (val.type()) {
    case QJsonValue::Double: return qRound(val.toDouble());
    default: THROW(key + ": bad number format");
    }
}

int JsonObj::loadInt(const QString& key, int def) const THROWS{
    return value(key).isUndefined() ? def : loadInt(key);
}

int JsonObj::loadUint(const QString& key) const THROWS {
    int num = loadInt(key);
    if (num < 0)
        THROW(key + ": bad number format");
    return num;
}

int JsonObj::loadUint(const QString& key, int def) const THROWS{
    return value(key).isUndefined() ? def : loadUint(key);
}

int JsonObj::loadPint(const QString& key) const {
    int num = loadUint(key);
    if (!(num > 0)) THROW("expecting positive number");
    return num;
}

int JsonObj::loadPint(const QString& key, int def) const {
    return value(key).isUndefined() ? def : loadPint(key);
}

qreal JsonObj::loadQreal(const QString& key) const THROWS {
    const QJsonValue& val = value(key);

    switch (val.type()) {
    case QJsonValue::Undefined:
        return NAN; // not present means not a number
    case QJsonValue::String: { // infinities stored as strings
        const QString& s = val.toString();
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

qreal JsonObj::loadQreal(const QString& key, qreal def) const THROWS{
    return value(key).isUndefined() ? def : loadQreal(key);
}

qreal JsonObj::loadPreal(const QString& key) const {
    qreal num = loadQreal(key);
    if (!(num > 0)) THROW("expecting positive number");
    return num;
}

qreal JsonObj::loadPreal(const QString& key, qreal def) const {
    return value(key).isUndefined() ? def : loadPreal(key);
}

bool JsonObj::loadBool(const QString& key) const THROWS {
    const QJsonValue& val = value(key);
    switch (val.type()) {
    case QJsonValue::Bool: return val.toBool();
    default: THROW(key + ": not a boolean");
    }
}

bool JsonObj::loadBool(const QString& key, bool def) const THROWS{
    return value(key).isUndefined() ? def : loadBool(key);
}

QString JsonObj::loadString(const QString& key) const THROWS {
    const QJsonValue& val = value(key);
    switch (val.type()) {
    case QJsonValue::String: return val.toString();
    default: THROW(key + ": not a string");
    }
}

QString JsonObj::loadString(const QString& key, const QString& def) const THROWS{
    return value(key).isUndefined() ? def : loadString(key);
}

Range JsonObj::loadRange(const QString& key) const THROWS {
    Range range;
    range.from_json(loadObj(key));
    return range;
}

IJ JsonObj::loadIJ(const QString& key) const THROWS {
    IJ ij;
    ij.from_json(loadObj(key));
    return ij;
}
