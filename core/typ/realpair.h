//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/realpair.h
//! @brief     Defines class qpair
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef REALPAIR_H
#define REALPAIR_H

#include "core/def/comparable.h"
#include "core/typ/json.h"

//! 2D point, reals
class qpair {
public:
    COMPARABLE(const qpair&);

    qpair() { invalidate(); }
    qpair(double x_, double y_) : x(x_), y(y_) {}

    void fromJson(const class JsonObj&);
    void invalidate(); // x,y <- Q_QNAN

    QJsonObject toJson() const;
    bool isValid() const { return !qIsNaN(x) && !qIsNaN(y); }

    double x, y;
};

#endif // REALPAIR_H
