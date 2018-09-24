//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/base/settings.h
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

#include "qcr/engine/cell.h"
#include <QSettings>

//! Used to save and retrieve the main window detector, and certain parameter settings.

class XSettings : public QSettings {
public:
    XSettings(const QString& group = "");
    ~XSettings();

    void setBool(const QString& key, QcrCell<bool>& cell);
    void getBool(const QString& key, bool& val);

    void setInt(const QString& key, QcrCell<int>& cell);
    void getInt(const QString& key, int& target);

    void setReal(const QString& key, QcrCell<double>& cell);
    void getReal(const QString& key, double& target);

    void setStr(const QString& key, QString& var) { var = value(key).toString(); }
};

#endif // SETTINGS_H
