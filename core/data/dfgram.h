//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/dfgram.h
//! @brief     Defines class Dfgram
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef DFGRAM_H
#define DFGRAM_H

#include "core/typ/cached.h"
#include "core/typ/curve.h"

class Dfgram;
class Polynom;
Polynom computeBgFit(const Dfgram*);

//! A curve, and associated fit parameters

class Dfgram {
public:
    Dfgram(Curve&& c);
    Dfgram(const Dfgram&) = delete;
    Dfgram(Dfgram&&) = default;
    const Curve curve;
    const Polynom& getBgFit() const { return bgFit.get(this); }
private:
    mutable Kached<Dfgram,Polynom> bgFit {&computeBgFit};
};

#endif // DFGRAM_H
