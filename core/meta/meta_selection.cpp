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
#include "core/meta/metadata.h"

MetaSelection::MetaSelection(bool on)
    : vec {Metadata::size(), on}
    , list {[this]()->std::vector<int>{
            std::vector<int> ret;
            for (int i=0; i<Metadata::size(); ++i)
                if (vec[i].val())
                    ret.push_back(i);
            return ret; }}
{}

void MetaSelection::set(int idx, bool on)
{
    vec[idx].setVal(on);
}
