// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/actions.h
//! @brief     Defines functions newTrigger, newToggle
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef ACTIONS_H
#define ACTIONS_H

#include "typ/str.h"

class QAction;

namespace gui {

QAction* newTrigger(rcstr text, rcstr iconFile="");
QAction* newToggle(rcstr text, bool value, rcstr iconFile="");

} // namespace gui

#endif // ACTIONS_H
