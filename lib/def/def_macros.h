// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      lib/def/def_macros.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef DEF_MACROS_H
#define DEF_MACROS_H

#include <QtGlobal>

#ifdef Q_CC_GNU

#pragma GCC diagnostic ignored "-Wpragmas"

// for clang static analyser
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#pragma GCC diagnostic ignored "-Wpadded"
#pragma GCC diagnostic ignored "-Wcomment"

#endif


// for class definitions

// trouble with templates in macros
#define COMMA ,

// alias for the current class; const reference to it
#define CLASS(cls)                                                                                 \
private:                                                                                           \
    using Cls = cls;                                                                               \
                                                                                                   \
public:                                                                                            \
    typedef Cls const& rc;

// alias for the super class
#define SUPER(cls)                                                                                 \
private:                                                                                           \
    using super = cls;

// alias for super class and an access method (if non-public inheritance)
#define WITH_SUPER(cls)                                                                            \
    SUPER(cls)                                                                                     \
public:                                                                                            \
    super const& sup() const { return *this; }                                                     \
    super& sup() { return *this; }


// exception specification macro
#ifdef Q_OS_WIN

#define THROWS

#else

#define THROWS noexcept(false)

#endif


#endif // DEF_MACROS_H
