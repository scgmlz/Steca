// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/ij.h
//! @brief     Defines class IJ
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef IJ_H
#define IJ_H

#include "def/comparable.h"
#include "def/macros.h"
#include <QJsonObject>

//! A pair of integers, for use as image coordinates
class IJ {
public:
    int i, j;

    IJ(); // (0,0)
    IJ(int, int);

    COMPARABLE(IJ const&)

    QJsonObject to_json() const;
    void from_json(class JsonObj const&) THROWS;
};

#endif // IJ_H
