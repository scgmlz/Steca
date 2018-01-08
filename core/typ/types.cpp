// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/types.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "core/typ/types.h"

QStringList const& normStrLst() {
    static QStringList strLst{
        "none", "monitor", "Δ monitor", "Δ time", "background",
    };

    return strLst;
}
