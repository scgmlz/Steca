// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/gamma_selection.h
//! @brief     Defines GammaSelection
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef GAMMA_SELECTION_H
#define GAMMA_SELECTION_H

#include "core/typ/range.h"

//! Supports different ways of setting a gamma range.

class GammaSelection {
public:
    GammaSelection(const Range& _fullRange);

    void setModeTakeAll();
    void setModeSlicing();
    void setModeMinMax();

    void setNumSlices(int);
    void selectSlice(int);
    void setRange(const Range&);

    Range range() const;
    Range fullRange() const;
    qreal min() const;
    qreal max() const;
    int numSlices() const;
    bool isModeMinMax() const();

private:
    enum class Mode { all, slicing, minmax } mode;
    Range fullRange_;
    Range range_;
    int numSlices_;
    int iSlice;
};

#endif // GAMMA_SELECTION_H
