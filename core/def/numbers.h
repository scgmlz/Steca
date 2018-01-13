// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/def/numbers.h
//! @brief     Defines types qint, preal, and some conversion functions
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

// adapted parts of https://github.com/Microsoft/GSL.git
// added some more

#ifndef NUMBERS_H
#define NUMBERS_H

#include "core/def/debug.h"
#include "core/def/macros.h"
#include <QtGlobal> // to define Q_OS_WIN

// casting signed <-> unsigned

inline int to_i(int u) { return u; }
inline int to_u(int i) { return i; }

// reals

#undef NAN
#undef INF

extern qreal const NAN; // silent nan
extern qreal const INF;

//! A positive real number of type qreal

#ifndef QT_NO_DEBUG

class preal {
public:
    explicit preal(qreal val) : val_(val) { debug::ensure(val>0); }

    operator qreal() const { return val_; }

private:
    qreal val_;
};

#else

typedef qreal preal;

#endif

#endif // NUMBERS_H
