// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/actions.h
//! @brief     Defines functions newTrigger, newToggle, and class Actions
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef ACTIONS_H
#define ACTIONS_H

#include "typ/str.h"
#include <QAction>

namespace gui {

class TheHub;

QAction* newTrigger(rcstr text, rcstr iconFile="");
QAction* newToggle(rcstr text, rcstr iconFile="");

} // namespace gui

#endif // ACTIONS_H
