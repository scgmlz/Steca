//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/pars/params.cpp
//! @brief     Implements class Params
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/pars/params.h"
#include "core/calc/peak_info.h" // dataTags

void Params::onMeta()
{
    QStringList tags = PeakInfo::dataTags(false);
    for (int i=0; i< (Metadata::numAttributes(false) - Metadata::numAttributes(true)); ++i)
        tags.removeLast(); // remove all tags that are not numbers

    diagramX.setTags(tags);
    diagramY.setTags(tags);
}
