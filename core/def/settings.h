// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/def/settings.h
//! @brief     Defines class XSettings
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

//! Used to save and retrieve the main window geometry, and certain parameter settings.

class XSettings : public QSettings {
public:
    XSettings(const QString& group = "");
    ~XSettings();

    bool readBool(const QString& key, bool def = false) { return value(key, def).toBool(); }
    void saveBool(const QString& key, bool val) { setValue(key, val); }

    int readInt(const QString& key, int def = 0);
    void saveInt(const QString& key, int val) { setValue(key, val); }

    qreal readReal(const QString& key, qreal def = 0);
    void saveReal(const QString& key, qreal val) { setValue(key, val); }

    QString readStr(const QString& key, const QString& def = "") {
        return value(key, def).toString(); }
    void saveStr(const QString& key, const QString& val) { setValue(key, val); }
};

#endif // SETTINGS_H
