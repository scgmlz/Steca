// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/ij.h
//! @brief     Defines struct IJ.
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef IJ_H
#define IJ_H

#include "def/comparable.h"
#include "def/macros.h"
#include "typ/exception.h"

namespace typ {

// 2D point, integers

class JsonObj;

struct IJ {
    int i, j;

    IJ(); // (0,0)
    IJ(int, int);

    COMPARABLE(IJ const&)

    JsonObj saveJson() const;
    void loadJson(JsonObj const&) THROWS;
};

} // namespace typ

#endif // IJ_H
