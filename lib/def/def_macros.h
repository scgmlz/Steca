// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      def_macros.h
//! @brief     Helper macros for defining types end else.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

// everyone should include this file somehow

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
#define CLS(cls) \
  private: using Cls = cls; public: typedef Cls const& rc;

// alias for the super class
#define SUPER(cls) \
  private: using super = cls;

// alias for super class and an access method (if non-public inheritance)
#define WITH_SUPER(cls) SUPER(cls)                   \
  public: super const& sup() const { return *this; } \
          super&       sup()       { return *this; }

// exception specification macro
#ifdef Q_OS_WIN

#define THROWS

#else

#define THROWS noexcept(false)

#endif

#endif // DEF_MACROS_H
