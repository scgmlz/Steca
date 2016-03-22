# STeCa2: Coding Guidelines

(GitHub-flavoured Markdown; a nice editor is here: https://stackedit.io/editor)

In principle following BornAgain (BA) where appropriate.

## Comments & doxygen

BA uses Qt-style doxygen tags. So do we in the boilerplate headers, otherwise
`///` or `///<`.

Doxygen comments in header files. Normal comments in source files.

Make code read like a story - then it does not need much commenting.
Use comments to express intentions, and what is not included in and
understandable from code. Be brief and concise.

## Templates

### header file

```
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

### source file

```
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

namespace core {
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
}
// eof
```

Notes:

- in @file: `<file name>` without the path component
- @brief is not needed in .cpp
- do prefer includes in .cpp file, forward declarations in .h

### class definition

```cpp
//! @class <name>

class SimpleFunction: public Function {
  SUPER(SimpleFunction,Function)
public:

};
```

The macro SUPER is super handy. It defines aliases:
- `thisClass` for the current class,
- `super` for the superclass.

## Qt:: & std::

The project is based in Qt. Therefore using (current, not deprecated) Qt classes
and other resources is preferable. In particular, use Qt collections (`QVector`,
`QHash`, `QMap`), smart pointers (`QScopedPointer`,`QSharedPointer`) templated
functions (`qMin`,`qBound`), types (`qreal`, `quint16`), etc.,
rather than the corresponding `std::` counterparts.

A notable exception is `std::sort` (and perhaps other std algorithms.

### QtCreator

Qt Creator - a modern IDE that supports Qt development - provides syntax-aware coloring, block highlight, fast switching between header and source files, fast navigation through code, type hierarchy and usage of symbols, integration with the debugger, etc. All that improves productivity and allows code formatting with less whitespace and ornamentation, such as:

## code formatting

### indentation and alignment

Indent by 2 spaces (no tabs!) (discuss - BA uses 4).

Align code in vertical columns. It looks good, and one can quickly spot
inconsistencies. Human brain is good in pattern-recognition, so why not use it?
(Indentation by 2 spaces helps that we do not quickly run off the right edge.)

Compare:
```
class Exception: public QException {
public:
    Exception(rcstr msg_) throw(): msg(msg_) {}
    Exception(Exception const& that) throw(): msg(that.msg) {}
    ~Exception() throw() {}

    Exception *clone() const;
    void raise() const;

    str msg;
};
```
with
```
class Exception: public QException {
public:
  Exception(rcstr msg_)             throw(): msg(msg_)      {}
  Exception(Exception const& that)  throw(): msg(that.msg)  {}
 ~Exception()                       throw()                 {}

  Exception *clone() const;
  void raise()       const;

  str msg;
};
```

### braces

Egyptian.

### spaces

Yes between a control statement and `(` and before `{`.
No between a function name and `(`.
Otherwise use spaces parsimoniously. Syntax-colouring and font-variation (bold,
italics) in IDEs visually separates elements that in the past had to be
separated by spaces. That way spaces and blank lines can be saved for where it
matters. The best (arguably) is: colour and font-coding provides the grain, while
more of text fits on one screen concisely.

```
int foo(int n) {
  for_i (n) {
    int square = i*i;
  }

  switch (i) {
  case 0:
    break;
  default:
    break;
  }
}
```

## naming conventions

### names

- Classes: camel case, begin with uppercase.
- Variables: camel case, begin with lowercase. Descriptive names, the longer the
  scope and life span, the longer the name.
- Permissible one-letter names: `i`, `n`, `x`, `y` etc.
- Permissible two-letter names: `sz` (size)
- Permissible three-letter names (or parts of names):
  `min`, `max`, `res` (result), `val` (value), `msg` (message), `add`,
  `rem` (remove), `del` (delete), `fun` (that's what we have; kidding, function)
  etc.
- Plurals: name classes and variables of collection (list, vector, etc.) types in plural
  form (`Range range`, `Ranges ranges`)

### member variables
  - no need to prefix member variables (with m_ and such)
  - no need to mark overriden virtual methods as `virtual`
    (nor `override`)

## code

### prefix ++

If there can be both, `++i` is better than `i++`, although people usually do the
opposite.

### postpositive `const`

C++ syntax allows `const` that modifies a type to either precede or follow the
type name. One can be English or French.

- English (pre-positive): red mill; constant integer, `const int`
- French (post-positive): moulin rouge; integer constant, `int const`

Would you rather eat English or French cuisine? Also, tt is productive to
organize code in simple visual patterns, such as aligned columns.

Compare:

```
      int a = 1;
const int b = 2;
```

with

```
int       a = 1;
int const b = 2;
```

Also, the `const`s that modify `this` and `*` are post-positive.
Why then not all `const`s? Compare:

```
class C {
  // const-int, star-const, foo-(this)-const - inconsistent
  const int * const foo() const;
};
```

with

```
class C {
  // int-const, star-const, foo-(this)-const - consistent
  int const * const foo() const;
};
```

Q.E.D.

## useful macros

### SUPER

### for_i, for_int

Use `for_i (n)` instead of the full idiomatic
```
for (int i=0, iEnd=(n); i<iEnd; ++i)
```
and `for_int (v,n)` to use some `v` instead of `i`.

## the choice between float and double

If saving space is required, use `float`.  If specifically 64b floating point is required, use `double`. (In both cases consider an appropriate `typedef`.) Otherwise, which is in most cases, use `qreal`.

## the choice between int and uint

If negative values cannot be, use `uint` to express it. There is a rub: in many
places in libraries (Qt etc.), int is used instead. The probable reason is that
with `uint`s one can be careful when it comes in comparisons. And then when we
use those, we must occasionally cast our `uint`s to `int`s or the other way.
Remember, when casting `int` to `uint`, it must be guaranteed that its value is
non-negative. When casting `uint` to `int`, one loses half the range (is 2
billion enough?).

## Exceptions

Exceptions happen; therefore write exception-safe code (did we mention `QScopedPointer`?) Throw an `Exception` if something goes wrong, namely if data are incorrect during import of data files. To do that, use `THROW` and `RUNTIME_CHECK` macros; mark yer functions that do throw exceptions with `THROWS`.

## Debug time

- `ASSERT`
- `TR`
- `WT`
- `NEVER_HERE`
- `NOT_YET`

## Naming of types

- lists: `_lst`, e.g. `typedef QStringList str_lst;`
- vectors: `_vec`, e.g.  `typedef QVector<qreal> qreal_vec;`, `typedef QVector<uint> uint_vec;`
- typedefs of primitive types: `_t`, e.g. `typedef float intens_t;`
- reference typedefs prefix with `r`, e.g. `rJson`
- reference const typedefs prefix with `rc`, e.g. `rcJson`

## `typedef` & `using`

Use `typedef` to type-define types (globally). Use `using` for local-scope aliases.

## auto

Be careful with `auto` (copy) v. `auto &` (reference).
Also use `auto const&` to make the point.

## class getter methods

Name getSomething() if they must calculate that something,
or if they should be used with caution;
else prefer to call plain accessors just something()
