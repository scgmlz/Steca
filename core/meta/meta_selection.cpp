//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/meta/meta_selection.cpp
//! @brief     Implements class MetaSelection
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/meta/meta_selection.h"
#include "qcr/base/debug.h"

namespace {

std::vector<int> computeList(const MetaSelection* ms)
{
    std::vector<int> ret;
    for (int i=0; i<ms->vec.size(); ++i)
        if (ms->vec[i].val())
            ret.push_back(i);
    return ret;
}

} // namespace

MetaSelection::MetaSelection()
    : list {&computeList}
{}

void MetaSelection::set(int idx, bool on)
{
    vec[idx].setVal(on);
    list.invalidate();
}
