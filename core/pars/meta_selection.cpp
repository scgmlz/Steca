//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/pars/meta_selection.h
//! @brief     Implements class MetaSelection
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/pars/meta_selection.h"
#include "core/raw/metadata.h"

MetaSelection::MetaSelection()
    : metaSelection_ (Metadata::size(), false)
{}

void MetaSelection::set(int idx, bool on)
{
    metaSelection_[idx] = on;
    // alternative representation:
    metaInfoNums_.clear();
    for (int i=0; i<Metadata::size(); ++i)
        if (metaSelection_[i])
            metaInfoNums_.push_back(i);
}
