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

#ifndef TESTS_H
#define TESTS_H

#ifdef TESTS
//------------------------------------------------------------------------------
// tests

#include "doctest.h"

#pragma GCC diagnostic ignored "-Wgnu-statement-expression"

#define TEST(name, code)  \
TEST_CASE(name) {         \
  code                    \
}

#else

#define TEST(name, code)  \

#endif

//------------------------------------------------------------------------------
#endif // TESTS_H
// eof
