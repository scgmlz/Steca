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
#include "gui/dialogs/exportfile_dialogfield.h"
#include "gui/mainwin.h"
#include <QGroupBox>
#include <QButtonGroup>

namespace {

// TODO move file saving code to Core

} // namespace


//  ***********************************************************************************************
//! @class ExportPolefig

ExportPolefig::ExportPolefig()
    : CModal("polefig")
    , QDialog(gGui)
{
    rbAll_.setChecked(true);
    bool interpolated = gSession->interpol().enabled();
    rbOriginalGrid_.setChecked(!interpolated);
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

void ExportPolefig::save()
{
    close();
}

/*
void PoleFiguresFrame::savePoleFigureOutput()
{
    int reflCount = gSession->peaks().count();
    ASSERT(reflCount); // user should not get here if no peak is defined
    QString path = tabSave_->path(false);
    if (path.isEmpty())
        THROW("cannot save pole figure: file path is empty");
    if (tabSave_->onlySelectedRefl()) {
        writePoleFigureOutputFiles(path, getReflIndex());
        return;
    }
    // all peaks
    for_i (reflCount) // TODO collect output into one message
        writePoleFigureOutputFiles(path, i);
}

static QString const OUT_FILE_TAG(".refl%1");
static int const MAX_LINE_LENGTH_POL(9);

void PoleFiguresFrame::writePoleFigureOutputFiles(const QString& filePath, int index)
{
    PeakInfos reflInfo;
    if (getInterpolated())
        reflInfo = interpPoints_.at(index);
    else
        reflInfo = calcPoints_.at(index);
    bool withFit = !gSession->peaks().at(index).isRaw();
    QString path = QString(filePath + OUT_FILE_TAG).arg(index + 1);
    bool check = false;
    int numSavedFiles = 0;

    if (tabSave_->outputInten()) {
        QVector<double> output;
        for_i (reflInfo.count())
            output.append(reflInfo.at(i).inten());
        const QString intenFilePath = path + ".inten";
        writeListFile(intenFilePath, reflInfo, output);
        writePoleFile(intenFilePath, reflInfo, output);
        writeErrorMask(intenFilePath, reflInfo, output);
        check = true;
        numSavedFiles += 3;
    }

    if (tabSave_->outputTth() && withFit) {
        QVector<double> output;
        for_i (reflInfo.count())
            output.append(reflInfo.at(i).tth());
        const QString tthFilePath = filePath + ".tth";
        writeListFile(tthFilePath, reflInfo, output);
        writePoleFile(tthFilePath, reflInfo, output);
        check = true;
        numSavedFiles += 2;
    }

    if (tabSave_->outputFWHM() && withFit) {
        QVector<double> output;
        for_i (reflInfo.count())
            output.append(reflInfo.at(i).fwhm());
        const QString fwhmFilePath = filePath + ".fwhm";
        writeListFile(fwhmFilePath, reflInfo, output);
        writePoleFile(fwhmFilePath, reflInfo, output);
        check = true;
        numSavedFiles += 2;
    }

    if (numSavedFiles > 0)
        qWarning() << "something went wrong, yet " << numSavedFiles << " files have been saved";
    else
        qWarning() << "no files saved";
}

void PoleFiguresFrame::writeErrorMask(
    const QString& filePath, PeakInfos reflInfo, const QVector<double>& output)
{
    QFile* file = file_dialog::openFileConfirmOverwrite("file", this, filePath);
    if (!file)
        return;
    QTextStream stream(file);

    for (int j = 0, jEnd = reflInfo.count(); j < jEnd; j += 9) {
        int max = j + MAX_LINE_LENGTH_POL;
        for (int i = j; i < max; i++) {
            if (qIsNaN(output.at(i)))
                stream << "0 ";
            else
                stream << "1 ";
        }
        stream << '\n';
    }
}

void PoleFiguresFrame::writePoleFile(
    const QString& filePath, PeakInfos reflInfo, const QVector<double>& output)
{
    QFile* file = file_dialog::openFileConfirmOverwrite("file", this, filePath);
    if (!file)
        return;
    QTextStream stream(file);

    for (int j = 0, jEnd = reflInfo.count(); j < jEnd; j += 9) {
        int max = j + MAX_LINE_LENGTH_POL;
        for (int i = j; i < max; i++) {
            if (qIsNaN(output.at(i)))
                stream << " -1  ";
            else
                stream << output.at(i) << " ";
        }
        stream << '\n';
    }
}

void PoleFiguresFrame::writeListFile(
    const QString& filePath, PeakInfos reflInfo, const QVector<double>& output)
{
    QFile* file = file_dialog::openFileConfirmOverwrite("file", this, filePath);
    QTextStream stream(file);

    for_i (reflInfo.count()) {
        stream << double(reflInfo.at(i).alpha()) << " " << double(reflInfo.at(i).beta()) << " "
               << output.at(i) << '\n';
    }
}
*/
