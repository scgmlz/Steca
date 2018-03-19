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
#include <iostream>

XSettings::XSettings(const QString& group) {
    beginGroup(group);
    std::cerr << "XSettings+ " << group.toLatin1().constData() << "\n";
}

XSettings::~XSettings() {
    std::cerr << "XSettings-\n";
    endGroup();
}

qreal XSettings::readReal(const QString& key, qreal def) {
    auto var = value(key, QVariant());
    bool ok;
    qreal val = var.toDouble(&ok);
    return ok ? val : def;
}

int XSettings::readInt(const QString& key, int def) {
    auto var = value(key, QVariant());
    bool ok;
    int val = var.toInt(&ok);
    return ok ? val : def;
}
