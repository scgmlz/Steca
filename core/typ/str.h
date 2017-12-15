// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/str.h
//! @brief     Defines str:=QString and rcstr.
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef STR_H
#define STR_H

#include <QString>

// string support
typedef QString str; // a short alias for the QString class
typedef str const& rcstr; // a reference to a string constant

#endif // STR_H
