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
//#include "core/base/async.h"
#include "core/base/exception.h"
#include "core/data/export.h"
#include "core/session.h"
#include "gui/mainwin.h"
//#include "qcr/base/debug.h"
//#include <qmath.h>

ExportDfgram::ExportDfgram()
    : DialogMultisave(gGui, "ExportDfgram", "Diffractogram export", data_export::defaultFormats,
                      "diffractogram", true)
{
}

void ExportDfgram::writeCurrent(QTextStream& stream)
{
    const Cluster* cluster = gSession->currentCluster();
    ASSERT(cluster);
    const Curve& curve = cluster->currentDfgram().curve;
    const QString separator = data_export::separator(format());
    data_export::writeCurve(stream, curve, cluster, cluster->rgeGma(), separator);
}

/*
void ExportDfgram::saveAll(QFile* file, const QString& format, ExportDfgram* parent)
{
    // In one-file mode, start output stream; in multi-file mode, only do prepations.
    const QString path = parent->fileField_->path(true, !file);
    if (path.isEmpty())
        return;
    QTextStream* stream = nullptr;
    int nClusters = gSession->activeClusters.size();
    ASSERT(nClusters>0);
    if (file) {
        stream = new QTextStream(file);
    } else {
        // check whether any of the numbered files already exists
        QStringList existingFiles;
        for (int i=0; i<nClusters; ++i) {
            QString currPath = data_export::numberedFileName(path, i, nClusters+1);
            if (QFile(currPath).exists())
                existingFiles << QFileInfo(currPath).fileName();
        }
        if (existingFiles.size() &&
            !file_dialog::confirmOverwrite(existingFiles.size()>1 ?
                                           "Files exist" : "File exists",
                                           parent, existingFiles.join(", ")))
            return;
    }
    TakesLongTime progress("save diffractograms", nClusters, &parent->fileField_->progressBar);
    int picNum = 0;
    int fileNum = 0;
    int nSlices = gSession->gammaSelection.numSlices.val();
    const QString separator = data_export::separator(format);
    for (const Cluster* cluster : gSession->activeClusters.clusters.yield()) {
        ++picNum;
        progress.step();
        for (int i=0; i<qMax(1,nSlices); ++i) {
            if (!file) {
                QFile* file = new QFile(data_export::numberedFileName(
                                            path, ++fileNum, nClusters+1));
                if (!file->open(QIODevice::WriteOnly | QIODevice::Text))
                    THROW("Cannot open file for writing: " + path);
                delete stream;
                stream = new QTextStream(file);
            }
            ASSERT(stream);
            const Range gmaStripe = gSession->gammaSelection.slice2range(cluster->rgeGma(), i);
            const Curve& curve = cluster->dfgrams.yield_at(i,cluster).curve;
            *stream << "Picture Nr: " << picNum << '\n';
            if (nSlices > 1)
                *stream << "Gamma slice Nr: " << i+1 << '\n';
            data_export::writeCurve(*stream, curve, cluster, gmaStripe, separator);
        }
    }
    delete stream;
}
*/
