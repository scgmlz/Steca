//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/misc/settings.h
//! @brief     Defines class XSettings
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

//! Used to save and retrieve the main window detector, and certain parameter settings.

class XSettings : public QSettings {
public:
    XSettings(const QString& group = "");
    ~XSettings();

    bool readBool(const QString& key, bool def = false);
    void getBool(const QString& key, bool& val);

    int readInt(const QString& key, int def = 0);
    void getInt(const QString& key, int& target);

    double readReal(const QString& key, double def = 0);
    void getReal(const QString& key, double& target);

    QString readStr(const QString& key, const QString& def = "") {
        return value(key, def).toString(); }
};

#endif // SETTINGS_H
