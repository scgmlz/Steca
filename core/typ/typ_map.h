// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/typ_map.h
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


namespace typ {

template <typename Key, typename T> class map : public QMap<Key, T> {
    using super_type = QMap<Key, T>;
private:
    using super = super_type;
public:
    using super::clear;
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
