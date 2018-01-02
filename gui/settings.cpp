// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/settings.cpp
//! @brief     Implements classes Settings, TheHub
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "settings.h"
#include "def/debug.h"
#include <QAction>
#include <QSpinBox>

namespace gui {

Settings::Settings(rcstr group) {
    setFallbacksEnabled(false);
    beginGroup(group);
}

Settings::~Settings() {
    endGroup();
}

QVariant Settings::readVariant(rcstr key, const QVariant& def) {
    auto val = value(key, def);
    return val;
}

void Settings::read(rcstr key, QAction* act, bool def) {
    debug::ensure(act->isCheckable());
    if (act)
        act->setChecked(readVariant(key, def).toBool());
}

void Settings::save(rcstr key, QAction* act) {
    debug::ensure(act->isCheckable());
    if (act)
        saveVariant(key, act->isChecked());
}

void Settings::read(rcstr key, QSpinBox* box, int def) {
    if (box)
        box->setValue(readVariant(key, def).toInt());
}

void Settings::save(rcstr key, QSpinBox* box) {
    if (box)
        saveVariant(key, box->value());
}

void Settings::read(rcstr key, QDoubleSpinBox* box, qreal def) {
    if (box)
        box->setValue(readVariant(key, def).toDouble());
}

void Settings::save(rcstr key, QDoubleSpinBox* box) {
    if (box)
        saveVariant(key, box->value());
}

qreal Settings::readReal(rcstr key, qreal def) {
    auto var = readVariant(key, QVariant());
    bool ok;
    qreal val = var.toDouble(&ok);
    return ok ? val : def;
}

int Settings::readInt(rcstr key, int def) {
    auto var = readVariant(key, QVariant());
    bool ok;
    int val = var.toInt(&ok);
    return ok ? val : def;
}

} // namespace gui
