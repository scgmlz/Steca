# STeCa2: Coding Guidelines

(GitHub-flavoured Markdown; a nice editor is here: https://stackedit.io/editor)

In principle following BornAgain (BA) where appropriate.

## header file template

```cpp
// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      <file name>
//! @brief     <brief description>
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef <GUARD>
#define <GUARD>

#include "other STeCa headers"
#include <Qt includes>

namespace <name> {
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
}

#endif //<GUARD>
```

## source file template

```cpp
// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_defs.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "header file"
#include "other STeCa2 headers"
#include <Qt includes>

// eof
```

Notes:

- in @file: `<file name>` without the path component
- @brief is not needed in .cpp
- do prefer includes in .cpp file, forward declarations in .h

## Qt:: & std::
The project is based in Qt. Therefore using (current, not deprecated) Qt classes and other resources is preferable. In particular, use Qt collections (`QVector`, `QHash`, `QMap`), smart pointers (`QScopedPointer`,`QSharedPointer`) templated functions (`qMin`,`qBound`), types (`qreal`, `quint16`), etc., rather than the corresponding `std::` counterparts.

## QtCreator
Qt Creator - a modern IDE that supports Qt development - provides syntax-aware coloring, block highlight, fast switching between header and source files, fast navigation through code, type hierarchy and usage of symbols, integration with the debugger, etc. All that improves productivity and allows code formatting with less whitespace and ornamentation, such as:

## code formatting

  - indentation by 2 spaces (no tabs!) (discuss - BA uses 4)
  - Egyptian braces
  - no need to prefix member variables (with m_ and such)
  - no need to mark overriden virtual methods as `virtual`

### postpositive `const`

C++ syntax allows `const` that modifies a type to either precede or follow the type name. The post-positive const is preferred, and here is why: it is like in French, first you say what we talk about, and then you point out its attribute:

- pre-positove English: red mill; constant integer, `const int`
- post-positive French: moulin rouge; integer constant, `int const`

It is productive to organize code in simple visual patterns - aligned columns. Compare:

```
      int a = 1;
const int b = 2;
```

with

```
int       a = 1;
int const b = 2;
```

And the `const`s that modify `this` and `*` are already only post-positive. Why not all then? Compare:

```
class C {
  const int * const f() const;
};
```

with

```
class C {
  // int-const, star-const, foo-const
  int const * const foo() const;
};
```

Q.E.D.

## class definition template

```cpp
class SimpleFunction: public Function {
  SUPER(SimpleFunction,Function)
public:

};
```

The macro SUPER is super handy. It defines aliases:
- `super` for the superclass,
- `thisClass` for the current class.

## other useful macros

Use `for_i (n)` instead of the full idiomatic
```
for (int i=0, iEnd=(n); i<iEnd; ++i)
```
and `for_int (v,n)` to use some `v` instead of `i`.

## floats, doubles

If saving space is required, use `float`.  If specifically 64b floating point is required, use `double`. (In both cases consider an appropriate `typedef`.) Otherwise, which is in most cases, use `qreal`.

## Exceptions

Exceptions happen; therefore write exception-safe code (did we mention `QScopedPointer`?) Throw an `Exception` if something goes wrong, namely if data are incorrect during import of data files. To do that, use `THROW` and `RUNTIME_CHECK` macros; mark yer functions that do throw exceptions with `THROWS`.

## Debug time

- `ASSERT`
- `TR`
- `WT`
- `NEVER_HERE`
- `NOT_YET`

## Naming

- lists: `_lst`, e.g. `typedef QStringList str_lst;`
- vectors: `_vec`, e.g.  `typedef QVector<qreal> qreal_vec;`, `typedef QVector<uint> uint_vec;`
