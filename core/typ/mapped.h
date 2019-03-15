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

#include <QString>
#include <map>

//! A map with keys of type QString.

class Mapped : private std::map<QString,double> {
    using super = std::map<QString,double>;
public:
    Mapped() {}
    void setDouble(const QString& key, double value) { super::at(key) = value; }
    bool has(const QString& key) const { return find(key)!=end(); }
    double at(const QString& key) const { return super::at(key); }
};

#endif // MAPPED_H
