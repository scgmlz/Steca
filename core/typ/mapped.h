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

class Mapped : public std::map<QString,double> {
public:
    Mapped() {}
    bool has(const QString& key) const { return find(key)!=end(); }
};

#endif // MAPPED_H
