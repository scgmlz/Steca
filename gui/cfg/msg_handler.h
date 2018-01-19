// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/cfg/msg_handler.h
//! @brief     Defines messageHandler.
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef MSG_HANDLER_H
#define MSG_HANDLER_H

#include "core/typ/str.h"

void messageHandler(QtMsgType type, QMessageLogContext const& ctx, rcstr msg);

#endif // MSG_HANDLER_H
