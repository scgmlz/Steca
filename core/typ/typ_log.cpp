// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/typ_log.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "typ_log.h"

void MessageLogger::info(rcstr msg) {
    if (handler)
        handler(msg, INFO);
}

void MessageLogger::warn(rcstr msg) {
    if (handler)
        handler(msg, WARN);
}

void MessageLogger::popup(rcstr msg) {
    if (handler)
        handler(msg, POPUP);
}

void (*MessageLogger::handler)(rcstr, eType) = nullptr;
