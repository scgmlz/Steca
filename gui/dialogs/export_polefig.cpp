//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/export_polefig.cpp
//! @brief     Implements class ExportPolefig
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/dialogs/export_polefig.h"
#include "core/base/async.h"
#include "core/data/collect_intensities.h"
#include "core/data/export.h"
#include "core/session.h"
#include "core/base/exception.h"
#include "core/data/export.h"
#include "gui/dialogs/dialog_save.h"
#include "gui/dialogs/file_dialog.h"
#include "gui/mainwin.h"
#include "qcr/base/debug.h"
#include <QGroupBox>
#include <algorithm>


//  ***********************************************************************************************
//! @class ExportPolefig

ExportPolefig::ExportPolefig()
    : DialogMultisave(
        gGui, "ExportPolefig", "Pole figure export",
        QStringList{"dat", "lst", "csv", "pol"}, "peak", gSession->peaks.size()>0)
{
}

int ExportPolefig::multiplicity()
{
    return gSession->peaks.size();
}

void ExportPolefig::writeCurrent(QTextStream& stream)
{
    data_export::writeInfoSequence(
        stream, *gSession->allPeaks.currentInfoSequence(), data_export::separator(format()));
}

void ExportPolefig::writeJointfile(QTextStream& stream)
{
    TakesLongTime progress("save pole figures", multiplicity(), &progressBar);
    int picNum = 0;
    for (const auto* peak : gSession->allPeaks.allInfoSequences()) {
        stream << "Peak Nr: " << ++picNum << '\n';
        data_export::writeInfoSequence(stream, *peak, data_export::separator(format()));
        progress.step();
    }
}

void ExportPolefig::writeOnefile(QTextStream& stream, const int idx)
{
    data_export::writeInfoSequence(
        stream, *gSession->allPeaks.currentInfoSequence(), data_export::separator(format()));
}
