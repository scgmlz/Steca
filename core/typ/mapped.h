//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/mapped.h
//! @brief     Defines and implements class Mapped
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

//! A map with keys of type QString and values of type Variant.

//! Used to hold metadata, coordinates, and fit outcomes.

class Mapped : private std::map<QString,QVariant> {
    using super = std::map<QString,QVariant>;
public:
    Mapped() {}
    template<typename T>
    void set(const QString& key, T value);
    bool has(const QString& key) const { return find(key)!=end(); }
    template<typename T>
    T get(const QString& key) const;
    QVariant at(const QString& key) const { return super::at(key); }
};

template<typename T>
void Mapped::set(const QString& key, T value)
{
    ASSERT(!has(key));
    super::operator[](key) = QVariant::fromValue<T>(value);
}

template<typename T>
T Mapped::get(const QString& key) const
{
    const QVariant& entry = super::at(key);
    ASSERT(entry.canConvert<T>());
    return entry.value<T>();
}

#endif // MAPPED_H
