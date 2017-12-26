// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/steca.cpp
//! @brief     Defines messageHandler.
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "typ/str.h"

extern class QMainWindow* pMainWin;
void messageHandler(QtMsgType type, QMessageLogContext const& ctx, rcstr msg);
