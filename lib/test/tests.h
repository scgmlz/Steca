// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      lib/test/tests.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //


#ifndef TESTS_H
#define TESTS_H

#ifdef TESTS
//------------------------------------------------------------------------------
// tests

#include "doctest.h"

#pragma GCC diagnostic ignored "-Wgnu-statement-expression"

#define TEST(name, code)  \
namespace {               \
TEST_CASE(name) {         \
  code                    \
}                         \
}

#else

#define TEST(name, code)  \

#endif

//------------------------------------------------------------------------------
#endif // TESTS_H
// eof
