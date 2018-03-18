// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/cfg/settings.cpp
//! @brief     Implements classes Settings, MainWin
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/cfg/settings.h"
#include "core/def/debug.h"

Settings::Settings(const QString& group) {
    beginGroup(group);
}

Settings::~Settings() {
    endGroup();
}

qreal Settings::readReal(const QString& key, qreal def) {
    auto var = value(key, QVariant());
    bool ok;
    qreal val = var.toDouble(&ok);
    return ok ? val : def;
}

int Settings::readInt(const QString& key, int def) {
    auto var = value(key, QVariant());
    bool ok;
    int val = var.toInt(&ok);
    return ok ? val : def;
}
