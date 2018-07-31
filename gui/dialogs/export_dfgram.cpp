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

#include "export_dfgram.h"
#include "core/algo/collect_intensities.h"
#include "core/def/idiomatic_for.h"
#include "core/session.h"
#include "core/aux/async.h"
#include "gui/dialogs/exportfile_dialogfield.h"
#include "gui/dialogs/file_dialog.h"
#include "gui/mainwin.h"
#include "qcr/base/debug.h"
#include <qmath.h>
#include <QGroupBox>
#include <QMessageBox>

namespace {

// TODO move file saving code to Core
void writeCurve(QTextStream& stream, const Curve& curve, const Cluster* cluster,
                const Range& rgeGma, const QString& separator)
{
    if (curve.isEmpty())
        qFatal("curve is empty");
    ASSERT(rgeGma.isValid());
    const Metadata& md = cluster->avgMetadata();
    stream << "Comment: " << md.comment << '\n';
    stream << "Date: " << md.date << '\n';
    stream << "Gamma range min: " << rgeGma.min << '\n';
    stream << "Gamma range max: " << rgeGma.max << '\n';

    for_i (Metadata::numAttributes(true))
        stream << Metadata::attributeTag(i, true) << ": "
               << md.attributeValue(i).toDouble() << '\n';

    stream << "Tth" << separator << "Intensity" << '\n';
    for_i (curve.xs().size())
        stream << curve.x(i) << separator << curve.y(i) << '\n';

    stream.flush(); // not sure whether we need this
}

//! Returns templatedName with '%d' replaced by string representation of num.

//!  The string representation of num has leading zeros, and its number of
//!  digits is determined by the maximum value maxNum.

QString numberedName(const QString& templatedName, int num, int maxNum) {
    if (!templatedName.contains("%d"))
        qFatal("path does not contain placeholder %%d");
    QString ret = templatedName;
    int nDigits = (int)log10((double)maxNum)+1;
    ret.replace("%d", QString("%1").arg(num, nDigits, 10, QLatin1Char('0')));
    qDebug() << "PATH " << templatedName << " -> " << ret;
    return ret;
}

} // namespace


//  ***********************************************************************************************
//! @class ExportDfgram

ExportDfgram::ExportDfgram()
    : QcrDialog(gGui, "Export diffractogram")
{
    rbAll_.programaticallySetValue(true);

    fileField_ = new ExportfileDialogfield(this, true, [this]()->void{save();});

    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Export diffractogram(s)");

    // layout
    auto* saveWhatLayout = new QVBoxLayout;
    saveWhatLayout->addWidget(&rbCurrent_);
    saveWhatLayout->addWidget(&rbAllSequential_);
    saveWhatLayout->addWidget(&rbAll_);

    auto* saveWhat = new QGroupBox {"Save what"};
    saveWhat->setLayout(saveWhatLayout);

    auto* vbox = new QVBoxLayout();
    vbox->addWidget(saveWhat);
    vbox->addLayout(fileField_);
    setLayout(vbox);
}

void ExportDfgram::save()
{
    try {
        if      (rbCurrent_.getValue())
            saveCurrent();
        else if (rbAllSequential_.getValue())
            saveAll(false);
        else if (rbAll_.getValue())
            saveAll(true);
        else
            qFatal("invalid case in ExportDfgram::save");
        close();
    } catch(Exception& ex) {
        qWarning() << "Could not save:\n" << ex.msg() << "\n";
    }
}

void ExportDfgram::saveCurrent()
{
    QFile* file = fileField_->file();
    if (!file)
        return;
    QTextStream stream(file);
    Cluster* cluster = gSession->dataset().highlight().cluster();
    ASSERT(cluster);
    const Curve& curve = cluster->currentGammaSector().curve();
    writeCurve(stream, curve, cluster, cluster->rgeGma(), fileField_->separator());
}

void ExportDfgram::saveAll(bool oneFile)
{
    const ActiveClusters& expt = gSession->activeClusters();
    // In one-file mode, start output stream; in multi-file mode, only do prepations.
    QString path = fileField_->path(true, !oneFile);
    if (path.isEmpty())
        return;
    QTextStream* stream = nullptr;
    if (oneFile) {
        QFile* file = fileField_->file();
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
            !file_dialog::confirmOverwrite(existingFiles.size()>1 ?
                                           "Files exist" : "File exists",
                                           this, existingFiles.join(", ")))
            return;
    }
    Progress progress(&fileField_->progressBar, "save diffractograms", expt.size());
    int picNum = 0;
    int fileNum = 0;
    int nSlices = gSession->gammaSelection().numSlices.val();
    for (Cluster* cluster : expt.clusters()) {
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
            const Curve& curve = cluster->gSectors.get(i).curve();
            *stream << "Picture Nr: " << picNum << '\n';
            if (nSlices > 1)
                *stream << "Gamma slice Nr: " << i+1 << '\n';
            writeCurve(*stream, curve, cluster, gmaStripe, fileField_->separator());
        }
    }
    delete stream;
}
