// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/def/settings.cpp
//! @brief     Implements class XSettings
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "core/def/settings.h"
#include "core/def/debug.h"

XSettings::XSettings(const QString& group)
{
    beginGroup(group);
}

XSettings::~XSettings()
{
    endGroup();
}

qreal XSettings::readReal(const QString& key, qreal def)
{
    auto var = value(key);
    bool ok;
    qreal val = var.toDouble(&ok);
    return ok ? val : def;
}

int XSettings::readInt(const QString& key, int def)
{
    auto var = value(key);
    bool ok;
    int val = var.toInt(&ok);
    return ok ? val : def;
}

//! Update target if settings have a double value with given key
void XSettings::getReal(const QString& key, qreal& target)
{
    auto var = value(key);
    bool ok;
    qreal val = var.toDouble(&ok);
    if (ok)
        target = val;
}

//! Update target if settings have an int value with given key
void XSettings::getInt(const QString& key, int& target)
{
    auto var = value(key);
    bool ok;
    qreal val = var.toInt(&ok);
    if (ok)
        target = val;
}
