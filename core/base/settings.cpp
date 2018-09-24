//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/base/settings.cpp
//! @brief     Implements class XSettings
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/base/settings.h"
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

void XSettings::setBool(const QString& key, QcrCell<bool>& cell)
{
    cell.setVal( value(key, cell.val()).toBool() );
}

void XSettings::setInt(const QString& key, QcrCell<int>& cell)
{
    bool ok;
    int val = value(key).toInt(&ok);
    if (ok)
        cell.setVal(val);
}

void XSettings::setReal(const QString& key, QcrCell<double>& cell)
{
    bool ok;
    double val = value(key).toDouble(&ok);
    if (ok)
        cell.setVal(val);
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
