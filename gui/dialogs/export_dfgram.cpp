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
#include "core/base/exception.h"
#include "core/data/export.h"
#include "core/session.h"
#include "gui/mainwin.h"
//#include "qcr/base/debug.h"
//#include <qmath.h>

ExportDfgram::ExportDfgram()
    : DialogMultisave(
        gGui, "ExportDfgram", "Diffractogram export", data_export::defaultFormats,
        "diffractogram", gSession->activeClusters.size() * gSession->gammaSelection.numSlices.val())
{
}

int ExportDfgram::multiplicity()
{
    return gSession->activeClusters.size() * gSession->gammaSelection.numSlices.val();
}

void ExportDfgram::writeCurrent(QTextStream& stream)
{
    const Cluster* cluster = gSession->currentCluster();
    ASSERT(cluster);
    const Curve& curve = cluster->currentDfgram().curve;
    data_export::writeCurve(
        stream, curve, cluster, cluster->rgeGma(), data_export::separator(format()));
}

void ExportDfgram::writeJointfile(QTextStream& stream)
{
    TakesLongTime progress("save diffractograms", multiplicity(), &progressBar);
    int fileNum = 0;
    const int nSlice = gSession->gammaSelection.numSlices.val();
    const int nCluster = gSession->activeClusters.size();
    const QString separator = data_export::separator(format());
    for (int iCluster=0; iCluster<nCluster; ++iCluster) {
        const Cluster* cluster = gSession->activeClusters.clusters.yield_at(iCluster);
        for (int iSlice=0; iSlice<qMax(1,nSlice); ++iSlice) {
            const QString fname = data_export::numberedFileName(path(), ++fileNum, nCluster+1);
            QFile file{fname};
            if (!file->open(QIODevice::WriteOnly | QIODevice::Text))
                THROW("Cannot open file for writing: " + path);
            QTextStream stream{file};
            const Range gmaStripe = gSession->gammaSelection.slice2range(
                cluster->rgeGma(), iSlice);
            const Curve& curve = cluster->dfgrams.yield_at(iSlice,cluster).curve;
            *stream << "Picture Nr: " << iCluster+1 << '\n';
            if (nSlice > 1)
                *stream << "Gamma slice Nr: " << iSlice+1 << '\n';
            data_export::writeCurve(*stream, curve, cluster, gmaStripe, separator);
            progress.step();
        }
    }
}

void ExportDfgram::writeOnefile(QTextStream& stream, const int idx)
{
    const int nSlices = gSession->gammaSelection.numSlices.val();

    const int iCluster = idx/nSlices;
    const Cluster* cluster = gSession->activeClusters.clusters.yield_at(iCluster);

    const int iSlice = idx%nSlices;
    const Range gmaStripe = gSession->gammaSelection.slice2range(cluster->rgeGma(), iSlice);
    const Curve& curve = cluster->dfgrams.yield_at(iSlice,cluster).curve;
    data_export::writeCurve(*stream, curve, cluster, gmaStripe, data_export::separator(format()));
}
