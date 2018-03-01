// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/capture_and_replay/enhance_widgets.cpp
//! @brief     Implements functions that return new Qt objects
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "enhance_widgets.h"
#include "gui/capture_and_replay/console.h"

// ************************************************************************** //
//  class CSettable
// ************************************************************************** //

CSettable::CSettable(const QString& name)
    : INamed(name)
{
    gConsole->learn(name, this);
}

CSettable::~CSettable() {
    gConsole->forget(name());
}

// ************************************************************************** //
//  class CModal
// ************************************************************************** //

CModal::CModal(const QString& name)
    : CSettable(name)
{
    gConsole->call("@push "+name);
}

CModal::~CModal()
{
    gConsole->log("@close");
    gConsole->call("@pop "+name());
}
