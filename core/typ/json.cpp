// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/json.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "def/exception.h"
#include "typ/ij.h"
#include "typ/qpair.h"
#include "typ/range.h"
#include "typ/str.h"
#include "typ/json.h"
#include <QStringList> // needed under Travis

namespace json_key {
str const I("i"), J("j"), X("x"), Y("y"), MIN("min"), MAX("max"), PARAMS("parameters"),
    TYPE("type"), FUN("f%1"), VALUE("value"), RANGE("range"), COUNT("count"), PEAK("guessed peak"),
    FWHM("guessed fwhm");
}

namespace typ {

JsonObj::JsonObj() {}

JsonObj::JsonObj(QJsonObject const& obj) : super(obj) {}

JsonObj& JsonObj::saveObj(rcstr key, JsonObj const& obj) {
    insert(key, obj.sup());
    return *this;
}

JsonObj JsonObj::loadObj(rcstr key, bool defEmpty) const THROWS {
    auto val = value(key);

    switch (val.type()) {
    case QJsonValue::Object: return val.toObject();
    case QJsonValue::Undefined:
        if (defEmpty)
            return JsonObj();
    // fallthrough
    default: THROW(key + ": not an object");
    }
}

JsonObj& JsonObj::saveArr(rcstr key, JsonArr const& arr) {
    insert(key, arr.sup());
    return *this;
}

JsonArr JsonObj::loadArr(rcstr key, bool defEmpty) const THROWS {
    auto val = value(key);

    switch (val.type()) {
    case QJsonValue::Array: return val.toArray();
    case QJsonValue::Undefined:
        if (defEmpty)
            return JsonArr();
    // fall through
    default: THROW(key + ": not an array");
    }
}

JsonObj& JsonObj::saveInt(rcstr key, int num) {
    insert(key, num);
    return *this;
}

int JsonObj::loadInt(rcstr key) const THROWS {
    auto val = value(key);

    switch (val.type()) {
    case QJsonValue::Double: return qRound(val.toDouble());
    default: THROW(key + ": bad number format");
    }
}

#define LOAD_DEF(type) value(key).isUndefined() ? def : load##type(key)

#define RET_LOAD_DEF(type) return LOAD_DEF(type);

int JsonObj::loadInt(rcstr key, int def) const THROWS{ RET_LOAD_DEF(Int) }

JsonObj& JsonObj::saveUint(rcstr key, uint num) {
    return saveInt(key, to_i(num));
}

uint JsonObj::loadUint(rcstr key) const THROWS {
    int num = loadInt(key);
    if (num < 0)
        THROW(key + ": bad number format");
    return to_u(num);
}

uint JsonObj::loadUint(rcstr key, uint def) const THROWS{ RET_LOAD_DEF(Uint) }

JsonObj& JsonObj::savePint(rcstr key, pint num) {
    return saveUint(key, num);
}

pint JsonObj::loadPint(rcstr key) const {
    uint num = loadUint(key);
    RUNTIME_CHECK(num > 0, "expecting positive number");
    return pint(num);
}

pint JsonObj::loadPint(rcstr key, uint def) const {
    return pint(LOAD_DEF(Pint));
}

static str const INF_P("+inf"), INF_M("-inf");

JsonObj& JsonObj::saveQreal(rcstr key, qreal num) {
    if (qIsNaN(num)) {
        // do not save anything for NaNs
    } else if (qIsInf(num)) {
        insert(key, num < 0 ? INF_M : INF_P);
    } else {
        insert(key, num);
    }

    return *this;
}

qreal JsonObj::loadQreal(rcstr key) const THROWS {
    auto val = value(key);

    switch (val.type()) {
    case QJsonValue::Undefined:
        return NAN; // not present means not a number
    case QJsonValue::String: { // infinities stored as strings
        auto s = val.toString();
        if (INF_P == s)
            return +INF;
        if (INF_M == s)
            return -INF;
        THROW(key + ": bad number format");
    }
    default: return val.toDouble();
    }
}

qreal JsonObj::loadQreal(rcstr key, qreal def) const THROWS{ RET_LOAD_DEF(Qreal) }

JsonObj& JsonObj::savePreal(rcstr key, preal num) {
    return saveQreal(key, num);
}

preal JsonObj::loadPreal(rcstr key) const {
    qreal num = loadQreal(key);
    RUNTIME_CHECK(num >= 0, "expecting positive number");
    return preal(num);
}

preal JsonObj::loadPreal(rcstr key, preal def) const { RET_LOAD_DEF(Preal) }

JsonObj& JsonObj::saveBool(rcstr key, bool b) {
    insert(key, b);
    return *this;
}

bool JsonObj::loadBool(rcstr key) const THROWS {
    auto val = value(key);

    switch (val.type()) {
    case QJsonValue::Bool: return val.toBool();
    default: THROW(key + ": not a boolean");
    }
}

bool JsonObj::loadBool(rcstr key, bool def) const THROWS{ RET_LOAD_DEF(Bool) }

JsonObj& JsonObj::saveString(rcstr key, rcstr s) {
    insert(key, s);
    return *this;
}

str JsonObj::loadString(rcstr key) const THROWS {
    auto val = value(key);

    switch (val.type()) {
    case QJsonValue::String: return val.toString();
    default: THROW(key + ": not a string");
    }
}

str JsonObj::loadString(rcstr key, rcstr def) const THROWS{ RET_LOAD_DEF(String) }

JsonObj& JsonObj::saveRange(rcstr key, Range const& range) {
    insert(key, range.saveJson());
    return *this;
}

Range JsonObj::loadRange(rcstr key) const THROWS {
    Range range;
    range.loadJson(loadObj(key));
    return range;
}

JsonObj& JsonObj::saveIJ(rcstr key, IJ const& ij) {
    insert(key, ij.saveJson());
    return *this;
}

IJ JsonObj::loadIJ(rcstr key) const THROWS {
    IJ ij;
    ij.loadJson(loadObj(key));
    return ij;
}

JsonObj& JsonObj::saveqpair(rcstr key, qpair const& qpair) {
    insert(key, qpair.saveJson());
    return *this;
}

qpair JsonObj::loadqpair(rcstr key) const THROWS {
    qpair qpair;
    qpair.loadJson(loadObj(key));
    return qpair;
}

JsonObj& JsonObj::operator+=(JsonObj const& that) {
    for (auto& key : that.keys())
        insert(key, that[key]);
    return *this;
}

JsonObj JsonObj::operator+(JsonObj const& that) const {
    return JsonObj(*this) += that;
}

JsonArr::JsonArr() {}

JsonArr::JsonArr(QJsonArray const& array) : super(array) {}

void JsonArr::append(JsonObj const& obj) {
    super::append(obj.sup());
}

uint JsonArr::count() const {
    return to_u(super::count());
}

JsonObj JsonArr::objAt(uint i) const {
    auto obj = super::at(to_i(i));
    RUNTIME_CHECK(QJsonValue::Object == obj.type(), "not an object at " + str::number(i));
    return super::at(to_i(i)).toObject();
}
}
