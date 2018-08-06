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

#include "export_polefig.h"
#include "core/algo/collect_intensities.h"
#include "core/def/idiomatic_for.h"
#include "core/session.h"
#include "gui/dialogs/exportfile_dialogfield.h"
#include "gui/mainwin.h"
#include "qcr/base/debug.h"
#include <QGroupBox>

namespace {

// TODO move file saving code to Core
void writePeakInfoInterpolated(QTextStream& stream)
{
    const PeakInfos& peakInfos = gSession->allPeaks.currentInterpolated();
    int col = 0;
    for (auto& info : peakInfos.peaks()) {
        double val = info.inten();
        if (qIsNaN(val))
            stream << " -1";
        else
            stream << val;
        if (++col==9) {
            stream  << '\n';
            col = 0;
        } else
            stream << " ";
    }
}

void writePeakInfoOriginalGrid(QTextStream& stream)
{
    const PeakInfos& peakInfos = gSession->allPeaks.currentDirect();
    for (auto& info : peakInfos.peaks()) {
        double val = info.inten();
        stream << double(info.alpha()) << " "
               << double(info.beta()) << " "
               << val << '\n';
    }
}

void writePeakInfo(QTextStream& stream, bool interpolated, const QString& separator)
{
    if (interpolated)
        writePeakInfoInterpolated(stream);
    else
        writePeakInfoOriginalGrid(stream);
}

} // namespace


//  ***********************************************************************************************
//! @class ExportPolefig

ExportPolefig::ExportPolefig()
    : QcrDialog(gGui, "Export Polefigure")
{
    if (false && gSession->peaks.count()>1) { // TODO restore once peak fits are cached
        exportCombi_.programaticallySetValue(true);
    } else {
        exportCurrent_.programaticallySetValue(true);
        exportMulti_.setEnabled(false);
        exportCombi_.setEnabled(false);
    }
    bool interpolated = gSession->params.interpolParams.enabled.val();
    gridOriginal_.programaticallySetValue(!interpolated);
    gridInterpol_.setEnabled(interpolated);
    gridInterpol_.programaticallySetValue(interpolated);

    fileField_ = new ExportfileDialogfield(this, true, [this]()->void{save();});

    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Export pole figure data");

    // layout
    auto* savePeaksLayout = new QVBoxLayout;
    savePeaksLayout->addWidget(&exportCurrent_);
    savePeaksLayout->addWidget(&exportMulti_);
    savePeaksLayout->addWidget(&exportCombi_);

    auto* savePeaks = new QGroupBox {"Save which peaks"};
    savePeaks->setLayout(savePeaksLayout);

    auto* saveGridLayout = new QVBoxLayout;
    saveGridLayout->addWidget(&gridOriginal_);
    saveGridLayout->addWidget(&gridInterpol_);

    auto* saveGrid = new QGroupBox {"Save which grid"};
    saveGrid->setLayout(saveGridLayout);

    auto* vbox = new QVBoxLayout();
    vbox->addWidget(savePeaks);
    vbox->addWidget(saveGrid);
    vbox->addLayout(fileField_);
    setLayout(vbox);
}

bool ExportPolefig::interpolated() {
    ASSERT(gridInterpol_.getValue() != gridOriginal_.getValue());
    return gridInterpol_.getValue();
}

void ExportPolefig::save()
{
    if      (exportCurrent_.getValue())
        saveCurrent();
/*    else if (exportMulti_.getValue())
        saveAll(false);
    else if (exportCombi_.getValue())
        saveAll(true);
*/
    else
        qFatal("invalid case in ExportPolefig::save");
    close();
}

void ExportPolefig::saveCurrent()
{
    QFile* file = fileField_->file();
    if (!file)
        return;
    QTextStream stream(file);
    const Cluster* cluster = gSession->highlightedCluster().cluster();
    ASSERT(cluster);
    const Curve& curve = algo::projectCluster(*cluster, cluster->rgeGma());
    if (curve.isEmpty())
        qFatal("curve is empty");
    writePeakInfo(stream, interpolated(), fileField_->separator());
}

// TODO: adapt from ExportDfgram, and activate it once peak fits are cached
/*
void ExportPolefig::saveAll(bool oneFile)
{
    const ActiveClusters& expt = gSession->activeClusters();
    // In one-file mode, start output stream; in multi-file mode, only do prepations.
    QString path = fileField_->path(true, !oneFile);
    if (path.isEmpty())
        return;
    QTextStream* stream = nullptr;
    if (oneFile) {
        QFile* file = file_dialog::openFileConfirmOverwrite("file", this, path);
        if (!file)
            return;
        stream = new QTextStream(file);
    } else {
        // check whether any of the numbered files already exists
        QStringList existingFiles;
        for_i (expt.size()) {
            QString currPath = numberedName(path, i, expt.size()+1);
            if (QFile(currPath).exists())
                existingFiles << QFileInfo(currPath).fileName();
        }
        if (existingFiles.size() &&
            QMessageBox::question(this, existingFiles.size()>1 ? "Files exist" : "File exists",
                                  "Overwrite files " + existingFiles.join(", ") + " ?") !=
            QMessageBox::Yes)
            return;
    }
    Progress progress(&fileField_->progressBar, "save diffractograms", expt.size());
    int picNum = 0;
    int fileNum = 0;
    int nSlices = gSession->gammaSelection.numSlices();
    for (const Cluster* cluster : expt.clusters()) {
        ++picNum;
        progress.step();
        for (int i=0; i<qMax(1,nSlices); ++i) {
            if (!oneFile) {
                QFile* file = new QFile(numberedName(path, ++fileNum, expt.size()+1));
                if (!file->open(QIODevice::WriteOnly | QIODevice::Text))
                    THROW("Cannot open file for writing: " + path);
                delete stream;
                stream = new QTextStream(file);
            }
            ASSERT(stream);
            const Range gmaStripe = gSession->gammaSelection.slice2range(i);
            const Curve& curve = cluster->toCurve(gmaStripe);
            ASSERT(!curve.isEmpty());
            *stream << "Picture Nr: " << picNum << '\n';
            if (nSlices > 1)
                *stream << "Gamma slice Nr: " << i+1 << '\n';
            writeCurve(*stream, curve, cluster, gmaStripe, fileField_->separator());
        }
    }
    delete stream;
}
*/
