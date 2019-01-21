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
    const Cluster* cluster = gSession->currentCluster();
    ASSERT(cluster);
    const Curve& curve = cluster->currentDfgram().curve;
    data_export::writeCurve(
        stream, curve, cluster, cluster->rgeGma(), data_export::separator(format()));
}

void ExportDfgram::writeOnefile(QTextStream& stream, const int idx)
{
    const int nSlices = gSession->gammaSelection.numSlices.val();
    const int iSlice = idx%nSlices;
    const int iCluster = idx/nSlices;

    const Cluster* cluster = gSession->activeClusters.clusters.yield().at(iCluster);
    const Range gmaStripe = gSession->gammaSelection.slice2range(cluster->rgeGma(), iSlice);
    const Curve& curve = cluster->dfgrams.yield_at(iSlice,cluster).curve;
    data_export::writeCurve(stream, curve, cluster, gmaStripe, data_export::separator(format()));
}

void ExportDfgram::writeJointfile(QTextStream& stream)
{
    TakesLongTime progress("save diffractograms", multiplicity(), &progressBar);
    const int nSlice = gSession->gammaSelection.numSlices.val();
    const int nCluster = gSession->activeClusters.size();
    const QString separator = data_export::separator(format());
    for (int iCluster=0; iCluster<nCluster; ++iCluster) {
        const Cluster* cluster = gSession->activeClusters.clusters.yield().at(iCluster);
        for (int iSlice=0; iSlice<qMax(1,nSlice); ++iSlice) {
            const Range gmaStripe = gSession->gammaSelection.slice2range(
                cluster->rgeGma(), iSlice);
            const Curve& curve = cluster->dfgrams.yield_at(iSlice,cluster).curve;
            stream << "Picture Nr: " << iCluster+1 << '\n';
            if (nSlice > 1)
                stream << "Gamma slice Nr: " << iSlice+1 << '\n';
            data_export::writeCurve(stream, curve, cluster, gmaStripe, separator);
            progress.step();
        }
    }
}
