// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/typ_strlst.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TYP_STRLST_H
#define TYP_STRLST_H

#include "def/numbers.h"
#include <QStringList>

// limited QStringList, only needed methods reexported

class str_lst : protected QStringList {
private:
    using super = QStringList;
public:
    str_lst() : super() {}
    str_lst(std::initializer_list<QString> l) : super(l) {}
    str_lst(super const& that) : super(that) {}
    super const& sup() const { return *this; }

    using super::count;
    using super::clear;
    using super::isEmpty;
    using super::begin;
    using super::end;
    using super::append;
    using super::removeLast;

    void append(str_lst const& that) { super::append(that); }

    QString const& at(uint i) const { return super::at(to_i(i)); }
};

#endif // TYP_STRLST_H
