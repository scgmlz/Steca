//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/misc/variant.cpp
//! @brief     Implements helper functions related to QVariant
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/misc/variant.h"
#include <QDate>

bool isNumeric(const QVariant& v)
{
    QMetaType::Type type = QMetaType::Type(v.type());

    switch (type) {
    case QMetaType::Int:
    case QMetaType::UInt:
    case QMetaType::LongLong:
    case QMetaType::ULongLong:
    case QMetaType::Double:
    case QMetaType::Long:
    case QMetaType::Short:
    case QMetaType::ULong:
    case QMetaType::UShort:
    case QMetaType::Float:
        return true;
    default:
        return false;
    }
}

#define RET_COMPARE_VALUES(v1, v2)                                      \
    if (v1 < v2)                                                        \
        return -1;                                                      \
    if (v1 > v2)                                                        \
        return +1;

#define IMPL_CMP(name, toType)                                          \
    int name(const QVariant& v1, const QVariant& v2) {                  \
        auto val1 = v1.toType(), val2 = v2.toType();                    \
        RET_COMPARE_VALUES(val1, val2)                                  \
        return 0;                                                       \
    }

IMPL_CMP(cmp_int, toInt)
IMPL_CMP(cmp_str, toString)
IMPL_CMP(cmp_date, toDate)

int cmp_real(const QVariant& v1, const QVariant& v2)
{
    double val1 = v1.toDouble(), val2 = v2.toDouble();
    if (qIsNaN(val1)) {
        return qIsNaN(val2) ? 0 : +1;
    }
    if (qIsNaN(val2)) {
        return -1;
    }
    RET_COMPARE_VALUES(val1, val2)
    return 0;
}

#undef IMPL_CMP
#undef RET_COMPARE_VALUES
