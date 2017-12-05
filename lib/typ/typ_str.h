// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      lib/typ/typ_str.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TYP_STR_H
#define TYP_STR_H

#include "def/def_macros.h"
#include <QString>
#include <QStringBuilder>

// string support
typedef QString     str;    // a short alias for the QString class
typedef str const&  rcstr;  // a reference to a string constant
typedef char const* pcstr;  // C-style (zero-byte-terminated) string

extern str const EMPTY_STR;  // an empty string

#endif  // TYP_STR_H
