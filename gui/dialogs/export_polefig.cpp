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
#include "core/session.h"
#include "core/algo/collect_intensities.h"
#include "core/def/idiomatic_for.h"
#include "gui/dialogs/exportfile_dialogfield.h"
#include "gui/mainwin.h"
#include <QGroupBox>
#include <QButtonGroup>

namespace {

// TODO move file saving code to Core
void writePeakInfoInterpolated(QTextStream& stream)
{
    const PeakInfos& peakInfos = gSession->interpolatedPeakInfos();
    const int LINE_LEN = 9;
    for (int j = 0, jEnd = peakInfos.size(); j < jEnd; j += LINE_LEN) {
        int max = j + LINE_LEN;
        for (int i = j; i < max; i++) {
            double val = peakInfos.at(i).inten();
            if (qIsNaN(val))
                stream << " -1  ";
            else
                stream << val << " ";
        }
        stream << '\n';
    }
}

void writePeakInfoOriginalGrid(QTextStream& stream)
{
    const PeakInfos& peakInfos = gSession->directPeakInfos();
    for_i (peakInfos.size()) {
        double val = peakInfos.at(i).inten();
        stream << double(peakInfos.at(i).alpha()) << " "
               << double(peakInfos.at(i).beta()) << " "
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
    : CModal("polefig")
    , QDialog(gGui)
{
    if (false && gSession->peaks().count()>1) { // TODO restore once peak fits are cached
        rbAll_.setChecked(true);
    } else {
        rbCurrent_.setChecked(true);
        rbAllSequential_.setEnabled(false);
        rbAll_.setEnabled(false);
    }
    bool interpolated = gSession->interpol().enabled();
    rbOriginalGrid_.setChecked(!interpolated);
    rbInterpolated_.setEnabled(interpolated);
    rbInterpolated_.setChecked(interpolated);

    fileField_ = new ExportfileDialogfield(this, true, [this]()->void{save();});

    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Export pole figure data");

    // layout
    auto* savePeaksLayout = new QVBoxLayout;
    savePeaksLayout->addWidget(&rbCurrent_);
    savePeaksLayout->addWidget(&rbAllSequential_);
    savePeaksLayout->addWidget(&rbAll_);

    auto* savePeaks = new QGroupBox {"Save which peaks"};
    savePeaks->setLayout(savePeaksLayout);

    auto* saveGridLayout = new QVBoxLayout;
    saveGridLayout->addWidget(&rbOriginalGrid_);
    saveGridLayout->addWidget(&rbInterpolated_);

    auto* saveGrid = new QGroupBox {"Save which grid"};
    saveGrid->setLayout(saveGridLayout);

    auto* vbox = new QVBoxLayout();
    vbox->addWidget(savePeaks);
    vbox->addWidget(saveGrid);
    vbox->addLayout(fileField_);
    setLayout(vbox);
}

bool ExportPolefig::interpolated() {
    ASSERT(rbInterpolated_.isChecked() != rbOriginalGrid_.isChecked());
    return rbInterpolated_.isChecked();
}

void ExportPolefig::save()
{
    if      (rbCurrent_.isChecked())
        saveCurrent();
/*    else if (rbAllSequential_.isChecked())
        saveAll(false);
    else if (rbAll_.isChecked())
        saveAll(true);
*/
    else
        qFatal("bug: invalid case in ExportPolefig::save");
    close();
}

void ExportPolefig::saveCurrent()
{
    QFile* file = fileField_->file();
    if (!file)
        return;
    QTextStream stream(file);
    const Cluster* cluster = gSession->dataset().highlight().cluster();
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
    int nSlices = gSession->gammaSelection().numSlices();
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
            const Range gmaStripe = gSession->gammaSelection().slice2range(i);
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
