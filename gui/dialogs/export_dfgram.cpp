//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/export_dfgram.cpp
//! @brief     Implements class ExportDfgram
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/dialogs/export_dfgram.h"
#include "core/base/async.h"
#include "core/calc/export.h"
#include "core/session.h"
#include "gui/mainwin.h"

ExportDfgram::ExportDfgram()
    : DialogMultisave(
        gGui, "ExportDfgram", "Diffractogram export", data_export::defaultFormats,
        "diffractogram", gSession->activeClusters.size()>0)
{
}

int ExportDfgram::multiplicity()
{
    return gSession->activeClusters.size() * gSession->gammaSelection.numSlices.val();
}

void ExportDfgram::writeCurrent(QTextStream& stream)
{
    data_export::writeCurrentDfgram(stream, format());
}

void ExportDfgram::writeOnefile(QTextStream& stream, const int idx)
{
    data_export::writeDfgram(stream, idx, format());
}

void ExportDfgram::writeJointfile(QTextStream& stream)
{
    TakesLongTime progress("save diffractograms", multiplicity(), &progressBar);
    data_export::writeAllDfgrams(stream, progress, format());
}
