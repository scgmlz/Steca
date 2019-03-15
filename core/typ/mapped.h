//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/mapped.h
//! @brief     Defines class Mapped (header-only)
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef MAPPED_H
#define MAPPED_H

#include "qcr/base/debug.h"
#include <QVariant>
#include <map>

//! A map with keys of type QString.

class Mapped : private std::map<QString,QVariant> {
    using super = std::map<QString,QVariant>;
public:
    Mapped() {}
    void setDouble(const QString& key, double value) {
        ASSERT(!has(key));
        super::operator[](key) = value; }
    bool has(const QString& key) const { return find(key)!=end(); }
    double at(const QString& key) const {
        const QVariant& entry = super::at(key);
        ASSERT(entry.canConvert(QMetaType::Double));
        return entry.value<double>(); }
};

#endif // MAPPED_H
