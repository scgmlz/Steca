/*******************************************************************************
 * STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#ifndef DEF_MACROS_H
#define DEF_MACROS_H
//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------
// for class definitions

// trouble with templates in macros
#define COMMA ,

// alias for the current class; const reference to it
#define CLASS(cls) \
  private: using Cls = cls; public: typedef Cls const& rc;

// alias for the super class
#define SUPER(cls) \
  private: using super = cls;

// alias for super class and an access method (if non-public inheritance)
#define WITH_SUPER(cls) SUPER(cls)                   \
  public: super const& sup() const { return *this; } \
          super&       sup()       { return *this; }

//------------------------------------------------------------------------------
// exception specification macro
#ifdef Q_OS_WIN

#define THROWS

#else

#define THROWS noexcept(false)

#endif

//------------------------------------------------------------------------------
#endif // DEF_MACROS_H
// eof
