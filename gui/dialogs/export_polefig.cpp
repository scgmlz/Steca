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
#include "core/aux/async.h"
#include "core/data/collect_intensities.h"
#include "core/session.h"
#include "core/aux/exception.h"
#include "gui/dialogs/subdialog_file.h"
#include "gui/dialogs/file_dialog.h"
#include "gui/mainwin.h"
#include "qcr/base/debug.h"
#include <QGroupBox>
#include <algorithm>

namespace {

// TODO move file saving code to Core

inline void writePeakInfo(QTextStream& stream, const PeakInfo &info, const QString &separator)
{
    stream << info.alpha() << separator
           << info.beta()  << separator
           << info.inten() << "\n";
}

void writeInfoSequence(QTextStream& stream, const InfoSequence &peakInfos, const QString &separator)
{
    for (auto& info : peakInfos.peaks()) {
        writePeakInfo(stream, info, separator);
    }
}

} // namespace


//  ***********************************************************************************************
//! @class ExportPolefig

ExportPolefig::ExportPolefig()
    : QcrDialog(gGui, "Export Polefigure")
{
    if (gSession->peaks.size()>1) { // TODO restore once peak fits are cached
        exportCombi_.programaticallySetValue(true);
    } else {
        exportCurrent_.programaticallySetValue(true);
        exportMulti_.setEnabled(false);
        exportCombi_.setEnabled(false);
    }

    fileField_ = new ExportfileDialogfield(this, true, [this]()->void{save();});

    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Export pole figure data");

    // layout
    auto* savePeaksLayout = new QVBoxLayout;
    savePeaksLayout->addWidget(&exportCurrent_);
    exportModeGroup.addButton(&exportCurrent_, 0);
    savePeaksLayout->addWidget(&exportMulti_);
    exportModeGroup.addButton(&exportMulti_, 1);
    savePeaksLayout->addWidget(&exportCombi_);
    exportModeGroup.addButton(&exportCombi_, 2);

    auto* savePeaks = new QGroupBox {"Save which peaks"};
    savePeaks->setLayout(savePeaksLayout);


    auto* vbox = new QVBoxLayout();
    vbox->addWidget(savePeaks);
    vbox->addLayout(fileField_);
    setLayout(vbox);
}

void ExportPolefig::save()
{
    int checkedId = exportModeGroup.checkedId();
    const auto exportMode = ExportMode(checkedId);
    const auto path = fileField_->path(true, exportMode==ExportMode::ALL_PEAKS_MULTIPLE_FILES);

    std::vector<InfoSequence const *> peaks;
    if (exportMode == ExportMode::CURRENT_PEAK) {
        peaks.push_back(gSession->allPeaks.currentInfoSequence());
    } else {
        peaks = gSession->allPeaks.allInfoSequences();
    }

    saveAll(! (exportMode==ExportMode::ALL_PEAKS_MULTIPLE_FILES), path, fileField_->separator(), peaks);

    close();
}


void saveOneFile(QString &path, const QString &separator, const std::vector<InfoSequence const *>
                 &peaks, TakesLongTime &progress)
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
        writeInfoSequence(stream, *peak, separator);
    }
}

// TODO: adapt from ExportDfgram, and activate it once peak fits are cached

void ExportPolefig::saveAll(bool oneFile, const QString &path, const QString &separator,
                            const std::vector<InfoSequence const *> &peaks)
{
    // In one-file mode, start output stream; in multi-file mode, only do prepations.
    if (path.isEmpty())
        return;

    QStringList paths;
    if (oneFile)
            paths << path;
    else for (int i=0; i<peaks.size(); ++i)
        paths << numberedFileName(path, i, peaks.size()+1);

    // check whether any of the file(s) already exists
    QStringList existingFiles;
    for (const auto &currPath : paths) {
        if (QFile::exists(currPath))
            existingFiles << QFileInfo(currPath).fileName();
    }
    if (existingFiles.size() &&
        !file_dialog::confirmOverwrite(existingFiles.size()>1 ?
                                       "Files exist" : "File exists",
                                       this, existingFiles.join(", ")))
        return;

    TakesLongTime progress("save diffractograms", peaks.size(), &fileField_->progressBar);

    if (paths.size() == 1) {
        saveOneFile(paths[0], separator, peaks, progress);
    } else {
        for (uint i=0; i<peaks.size(); ++i)  {
            saveOneFile(paths[i], separator, { peaks[i] }, progress);
        }

    }
}

