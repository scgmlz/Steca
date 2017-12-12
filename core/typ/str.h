// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/str.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef STR_H
#define STR_H

#include <QStringBuilder>

// string support
typedef QString str; // a short alias for the QString class
typedef str const& rcstr; // a reference to a string constant
typedef char const* pcstr; // C-style (zero-byte-terminated) string

#endif // STR_H
