// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/json.h
//! @brief     Defines class JsonObj
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef JSON_H
#define JSON_H

#include "core/def/numbers.h"
#include "core/typ/str.h"
#include <QJsonObject>

QJsonValue qreal_to_json(const qreal num);

class Range;
class IJ;

//! Extends QJsonObject by read-out methods; used to load a session from a .ste file
class JsonObj : public QJsonObject {
private:
    using super = QJsonObject;
public:
    JsonObj();
    JsonObj(QJsonObject const&);

    super const& sup() const { return *this; }

    JsonObj loadObj(rcstr key, bool defEmpty = false) const THROWS;

    QJsonArray loadArr(rcstr key, bool defEmpty = false) const THROWS;

    int loadInt(rcstr key) const THROWS;
    int loadInt(rcstr key, int def) const THROWS;

    int loadUint(rcstr key) const THROWS;
    int loadUint(rcstr key, int def) const THROWS;

    int loadPint(rcstr key) const THROWS;
    int loadPint(rcstr key, int def) const THROWS;

    qreal loadQreal(rcstr key) const THROWS;
    qreal loadQreal(rcstr key, qreal def) const THROWS;

    preal loadPreal(rcstr key) const THROWS;
    preal loadPreal(rcstr key, preal def) const THROWS;

    bool loadBool(rcstr key) const THROWS;
    bool loadBool(rcstr key, bool def) const THROWS;

    str loadString(rcstr key) const THROWS;
    str loadString(rcstr key, rcstr def) const THROWS;

    Range loadRange(rcstr key) const THROWS;

    IJ loadIJ(rcstr key) const THROWS;
};

#endif // JSON_H
