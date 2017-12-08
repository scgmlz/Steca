// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/typ_ij.h
//! @brief     Defines struct IJ.
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TYP_IJ_H
#define TYP_IJ_H

#include "def/def_cmp.h"
#include "def/exception.h"
#include "def/macros.h"

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

#endif // TYP_IJ_H
