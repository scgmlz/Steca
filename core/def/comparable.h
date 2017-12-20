// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/def/comparable.h
//! @brief     Defines macro COMPARABLE, which defines operators == and !=
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef COMPARABLE_H
#define COMPARABLE_H

#define COMPARABLE(rc)         \
    int compare(rc) const;     \
    bool operator==(rc) const; \
    bool operator!=(rc) const;

#endif // COMPARABLE_H
