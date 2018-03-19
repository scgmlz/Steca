// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/ij.h
//! @brief     Defines class IJ
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef IJ_H
#define IJ_H

#include "core/def/comparable.h"
#include "core/def/macros.h"
#include <QJsonObject>

//! A pair of integers, for use as image coordinates
class IJ {
public:
    int i, j;

    IJ(); // (0,0)
    IJ(int, int);

    COMPARABLE(const IJ&)

    QJsonObject toJson() const;
    void fromJson(const class JsonObj&) THROWS;
};

#endif // IJ_H
