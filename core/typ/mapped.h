//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/mapped.h
//! @brief     Defines class Mapped
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
    void set(const QString& key, double value);
    bool has(const QString& key) const;
    double doubleAt(const QString& key) const;
};

#endif // MAPPED_H
