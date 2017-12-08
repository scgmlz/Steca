// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/typ_json.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TYP_JSON_H
#define TYP_JSON_H

#include "def/def_gsl.h"
#include "typ/typ_str.h"
#include <QJsonArray>
#include <QJsonObject>

namespace json_key {
extern str const I, J, X, Y, MIN, MAX, PARAMS, TYPE, FUN, VALUE, RANGE, COUNT, PEAK, FWHM;
}

class qpair;

namespace typ {

class JsonArr;
struct Range;
struct IJ;

class JsonObj : protected QJsonObject {
private:
    using super = QJsonObject;
public:
    JsonObj();
    JsonObj(QJsonObject const&);

    super const& sup() const { return *this; }

    JsonObj& saveObj(rcstr key, JsonObj const&);
    JsonObj loadObj(rcstr key, bool defEmpty = false) const THROWS;

    JsonObj& saveArr(rcstr key, JsonArr const&);
    JsonArr loadArr(rcstr key, bool defEmpty = false) const THROWS;

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

    JsonObj& saveRange(rcstr key, Range const&);
    Range loadRange(rcstr key) const THROWS;

    JsonObj& saveIJ(rcstr key, IJ const&);
    IJ loadIJ(rcstr key) const THROWS;

    JsonObj& saveqpair(rcstr key, qpair const&);
    qpair loadqpair(rcstr key) const THROWS;

    JsonObj& operator+=(JsonObj const&);
    JsonObj operator+(JsonObj const&) const;
};

class JsonArr : protected QJsonArray {
private:
    using super = QJsonArray;
public:
    JsonArr();
    JsonArr(QJsonArray const&);

    super const& sup() const { return *this; }

    using super::append;
    void append(JsonObj const&);

    using super::begin;
    using super::end;

    uint count() const;
    JsonObj objAt(uint) const;
};

} // namespace typ

#endif // TYP_JSON_H
