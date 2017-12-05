// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/dock_help.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //


#ifndef DOCK_HELP_H
#define DOCK_HELP_H

#include "panel.h"

class QTextBrowser;

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class DockHelp : public DockWidget, protected RefHub {
  CLASS(DockHelp) SUPER(DockWidget)
public:
  DockHelp(TheHub&);

private:
  QTextBrowser *browser_;
};

//------------------------------------------------------------------------------
}}
#endif // DOCK_HELP_H
