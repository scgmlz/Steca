// ************************************************************************** //
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
// ************************************************************************** //

#ifndef REALPAIR_H
#define REALPAIR_H

#include "core/def/comparable.h"

//! 2D point, reals
class qpair {
    public:

    qreal x, y;

    qpair() { invalidate(); }
    qpair(qreal x_, qreal y_) : x(x_), y(y_) {}

    COMPARABLE(const qpair&);

    void invalidate(); // x,y <- NAN
    bool isValid() const { return !qIsNaN(x) && !qIsNaN(y); }

    QJsonObject toJson() const;
    void fromJson(const class JsonObj&) THROWS;
};

#endif // REALPAIR_H
