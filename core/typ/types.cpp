// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/types.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "typ/types.h"

QStringList const& normStrLst() {
    static QStringList strLst{
        "none", "monitor", "Δ monitor", "Δ time", "background",
    };

    return strLst;
}
