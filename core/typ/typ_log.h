// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/typ_log.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TYP_LOG_H
#define TYP_LOG_H

#include "def/defs.h"

class MessageLogger {
public:
    enum eType { INFO, WARN, POPUP };
    static void info(rcstr);
    static void warn(rcstr);
    static void popup(rcstr);

    static void (*handler)(rcstr, eType);
};

#endif // TYP_LOG_H
