//  ***********************************************************************************************
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
//  ***********************************************************************************************

#ifndef JSON_H
#define JSON_H

#include <QJsonObject>

QJsonValue double_to_json(const double num);

class Range;

//! Extends QJsonObject by read-out methods; used to load a session from a .ste file
class JsonObj : public QJsonObject {
public:
    JsonObj() {}
    JsonObj(const QJsonObject&);

    JsonObj loadObj(const QString& key, bool defEmpty = false) const;

    QJsonArray loadArr(const QString& key, bool defEmpty = false) const;

    int loadInt(const QString& key) const;
    int loadInt(const QString& key, int def) const;

    int loadUint(const QString& key) const;
    int loadUint(const QString& key, int def) const;

    int loadPint(const QString& key) const;
    int loadPint(const QString& key, int def) const;

    double loadQreal(const QString& key) const;
    double loadQreal(const QString& key, double def) const;

    double loadPreal(const QString& key) const;
    double loadPreal(const QString& key, double def) const;

    bool loadBool(const QString& key) const;
    bool loadBool(const QString& key, bool def) const;

    QString loadString(const QString& key) const;
    QString loadString(const QString& key, const QString& def) const;

    Range loadRange(const QString& key) const;
};

#endif // JSON_H
