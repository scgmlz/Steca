// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      lib/typ/typ_map.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TYP_MAP_H
#define TYP_MAP_H

#include "def/def_macros.h"
#include <QMap>

namespace typ {

template <typename Key, typename T> class map : public QMap<Key, T> {
    using super_type = QMap<Key, T>;
    CLASS(map) SUPER(super_type) public : using super::clear;
    using super::insert;
    using super::insertMulti;
    using super::remove;
    using super::find;
    using super::contains;
    using super::value;
    using super::take;
    using super::begin;
    using super::end;
    using super::cbegin;
    using super::cend;
};

template <typename Key, typename T> class owning_map : public map<Key, T*> {
public:
    ~owning_map() {
        for (auto* v : QMap<Key, T*>::values())
            delete v;
    }
};


}
#endif // TYP_MAP_H
