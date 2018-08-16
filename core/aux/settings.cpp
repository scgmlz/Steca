//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/aux/settings.cpp
//! @brief     Implements class XSettings
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/aux/settings.h"
//#include "qcr/base/debug.h"

XSettings::XSettings(const QString& group)
{
    // qDebug() << "XSettings BEG " << group;
    beginGroup(group);
}

XSettings::~XSettings()
{
    endGroup();
    // qDebug() << "XSettings END";
}

bool XSettings::readBool(const QString& key, bool def)
{
    return value(key, def).toBool();
}

int XSettings::readInt(const QString& key, int def)
{
    auto var = value(key);
    bool ok;
    int val = var.toInt(&ok);
    return ok ? val : def;
}

double XSettings::readReal(const QString& key, double def)
{
    auto var = value(key);
    bool ok;
    double val = var.toDouble(&ok);
    return ok ? val : def;
}

//! Update target if settings have a bool value with given key
void XSettings::getBool(const QString& key, bool& target)
{
    target = value(key).toBool();
}

//! Update target if settings have an int value with given key
void XSettings::getInt(const QString& key, int& target)
{
    auto var = value(key);
    bool ok;
    double val = var.toInt(&ok);
    if (ok)
        target = val;
}

//! Update target if settings have a double value with given key
void XSettings::getReal(const QString& key, double& target)
{
    auto var = value(key);
    bool ok;
    double val = var.toDouble(&ok);
    if (ok)
        target = val;
}
