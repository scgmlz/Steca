// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/map.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef MAP_H
#define MAP_H


namespace typ {

template <typename Key, typename T> class map : public QMap<Key, T> {
    using super_type = QMap<Key, T>;
private:
public:
};

template <typename Key, typename T> class owning_map : public map<Key, T*> {
public:
    ~owning_map() {
        for (auto* v : QMap<Key, T*>::values())
            delete v;
    }
};
}
#endif // MAP_H
