// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/def/def_cmp.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef DEF_CMP_H
#define DEF_CMP_H

#define COMPARABLE(rc)         \
    int compare(rc) const;     \
    bool operator==(rc) const; \
    bool operator!=(rc) const;

#endif // DEF_CMP_H
