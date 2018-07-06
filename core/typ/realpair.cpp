//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/realpair.cpp
//! @brief     Implements class qpair
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  Unit tests in test002_qpair
//
//  ***********************************************************************************************

#include "realpair.h"
#include "core/def/comparators.h"
#include "qcr/base/debug.h"

int qpair::compare(const qpair& that) const
{
    ASSERT(isValid() && that.isValid());
    RET_COMPARE_VALUE(x)
    RET_COMPARE_VALUE(y)
    return 0;
}

VALID_EQ_NE_OPERATOR(qpair)

void qpair::invalidate()
{
    x = y = Q_QNAN;
}

QJsonObject qpair::toJson() const
{
    return { { "x", double_to_json(x) }, { "y", double_to_json(y) } };
}

void qpair::fromJson(const JsonObj& obj)
{
    x = obj.loadQreal("x");
    y = obj.loadQreal("y");
}
