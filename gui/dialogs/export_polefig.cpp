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
        data_export::defaultFormats+QStringList{"pol"}, "peak", gSession->peaks.size()>0)
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
}

void ExportPolefig::writeOnefile(QTextStream& stream, const int idx)
{
}

/*
void ExportPolefig::save(QFile*, const QString& format, QcrDialog* parent)
{
    auto* myParent = static_cast<ExportPolefig*>(parent);
    const DialogMultisave::ExportMode exportMode = fileField_->exportMode();
    const QString path = myParent->fileField_->path(
        true, exportMode==DialogMultisave::ExportMode::ALL_PEAKS_MULTIPLE_FILES);

    std::vector<InfoSequence const *> peaks;
    if (exportMode == DialogMultisave::ExportMode::CURRENT_PEAK)
        peaks.push_back(gSession->allPeaks.currentInfoSequence());
    else
        peaks = gSession->allPeaks.allInfoSequences();

    const QString separator = data_export::separator(format);
    saveAll(!(exportMode==DialogMultisave::ExportMode::ALL_PEAKS_MULTIPLE_FILES),
            path, separator, peaks, parent);
}


void saveOneFile(QString& path, const QString& separator,
                 const std::vector<const InfoSequence*>& peaks, TakesLongTime& progress)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        THROW("Cannot open file for writing: " + path);
    QTextStream stream(&file);

    int picNum = 0;
    for (const auto peak : peaks) {
        ++picNum;
        progress.step();
        if (peaks.size() > 1)
            stream << "Picture Nr: " << picNum << '\n';
        data_export::writeInfoSequence(stream, *peak, separator);
    }
}

// TODO: adapt from ExportDfgram, and activate it once peak fits are cached

void ExportPolefig::saveAll(
    bool oneFile, const QString& path, const QString& separator,
    const std::vector<const InfoSequence*>& peaks, QcrDialog* parent)
{
    auto* myParent = static_cast<ExportPolefig*>(parent);
    // In one-file mode, start output stream; in multi-file mode, only do prepations.
    if (path.isEmpty())
        return;

    QStringList paths;
    if (oneFile)
        paths << path;
    else for (int i=0; i<peaks.size(); ++i)
        paths << data_export::numberedFileName(path, i, peaks.size()+1);

    // check whether any of the file(s) already exists
    QStringList existingFiles;
    for (const auto &currPath : paths) {
        if (QFile::exists(currPath))
            existingFiles << QFileInfo(currPath).fileName();
    }
    if (existingFiles.size() &&
        !file_dialog::confirmOverwrite(existingFiles.size()>1 ?
                                       "Files exist" : "File exists",
                                       myParent, existingFiles.join(", ")))
        return;

    TakesLongTime progress("save polefigures", peaks.size(), &myParent->fileField_->progressBar);

    if (paths.size() == 1) {
        saveOneFile(paths[0], separator, peaks, progress);
    } else {
        for (uint i=0; i<peaks.size(); ++i)
            saveOneFile(paths[i], separator, { peaks[i] }, progress);
    }
}
*/
