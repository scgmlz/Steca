// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/map.h
//! @brief     Defines templated class map
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef MAP_H
#define MAP_H

template <typename Key, typename T> class map : public QMap<Key, T> {
private:
public:
};

#endif // MAP_H
