// ************************************************************************** //
//
//  Steca2:    StressTextureCalculator ver. 2
//
//! @file      typ_str.h
//! @brief     Veneered QT class.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef TYP_STR_H
#define TYP_STR_H

#include "def/def_macros.h"
#include <QString>
#include <QStringBuilder>

//------------------------------------------------------------------------------

// string support
typedef QString     str;        // a short alias for the QString class
typedef str const&  rcstr;      // a reference to a string constant
typedef char const* pcstr;      // C-style (zero-byte-terminated) string

extern  str const   EMPTY_STR;  // an empty string

//------------------------------------------------------------------------------
#endif // TYP_STR_H
