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
#include <QString>
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

    JsonObj loadObj(const QString& key, bool defEmpty = false) const THROWS;

    QJsonArray loadArr(const QString& key, bool defEmpty = false) const THROWS;

    int loadInt(const QString& key) const THROWS;
    int loadInt(const QString& key, int def) const THROWS;

    int loadUint(const QString& key) const THROWS;
    int loadUint(const QString& key, int def) const THROWS;

    int loadPint(const QString& key) const THROWS;
    int loadPint(const QString& key, int def) const THROWS;

    qreal loadQreal(const QString& key) const THROWS;
    qreal loadQreal(const QString& key, qreal def) const THROWS;

    qreal loadPreal(const QString& key) const THROWS;
    qreal loadPreal(const QString& key, qreal def) const THROWS;

    bool loadBool(const QString& key) const THROWS;
    bool loadBool(const QString& key, bool def) const THROWS;

    QString loadString(const QString& key) const THROWS;
    QString loadString(const QString& key, const QString& def) const THROWS;

    Range loadRange(const QString& key) const THROWS;

    IJ loadIJ(const QString& key) const THROWS;
};

#endif // JSON_H
