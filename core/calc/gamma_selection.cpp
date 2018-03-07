// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/gamma_selection.cpp
//! @brief     Implements GammaSelection
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gamma_selection.h"
#include "core/session.h"

GammaSelection::GammaSelection()
{
    update();
    setModeTakeAll();
}

void GammaSelection::update()
{
}

void GammaSelection::setModeTakeAll()
{
    mode_ = Mode::all;
    range_ = fullRange_;
    iSlice_ = 0;
    emit gSession->sigGamma();
}

void GammaSelection::setModeSlicing()
{
    mode_ = Mode::slicing;
    selectSlice(1);
}

void GammaSelection::setModeMinMax()
{
    mode_ = Mode::all;
    range_ = fullRange_;
    iSlice_ = 0;
    emit gSession->sigGamma();
}

void GammaSelection::setNumSlices(int n)
{
    if (n<=0) {
        setModeTakeAll();
    } else {
        setModeSlicing();
        numSlices_ = qMax(n, 1);
        selectSlice(iSlice_);
    }
}

void GammaSelection::selectSlice(int i)
{
    setModeSlicing();
    iSlice_ = i;
    range_ = fullRange_.slice(iSlice_, numSlices_);
    emit gSession->sigGamma();
}

void GammaSelection::setRange(const Range& r)
{
    setModeMinMax();
    range_ = r;
}
