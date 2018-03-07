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
    GammaSelection();

    void update();

    void setModeTakeAll();
    void setModeSlicing();
    void setModeMinMax();

    void setNumSlices(int);
    void selectSlice(int);
    void setRange(const Range&);

    Range range() const { return range_; }
    qreal min() const { return range_.min; }
    qreal max() const { return range_.max; }
    int numSlices() const { return numSlices_; }
    int iSlice() const { return iSlice_; }
    bool isModeMinMax() const { return mode_==Mode::minmax; }

private:
    enum class Mode { all, slicing, minmax } mode_ {Mode::all};
    Range fullRange_;
    Range range_;
    int numSlices_ {1};
    int iSlice_;
};

#endif // GAMMA_SELECTION_H
