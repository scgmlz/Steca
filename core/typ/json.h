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

    JsonObj loadObj(rcstr key, bool defEmpty = false) const THROWS;

    QJsonArray loadArr(rcstr key, bool defEmpty = false) const THROWS;

    int loadInt(rcstr key) const THROWS;
    int loadInt(rcstr key, int def) const THROWS;

    uint loadUint(rcstr key) const THROWS;
    uint loadUint(rcstr key, uint def) const THROWS;

    pint loadPint(rcstr key) const THROWS;
    pint loadPint(rcstr key, uint def) const THROWS;

    JsonObj& saveQreal(rcstr key, qreal);
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

} // namespace typ

#endif // JSON_H
