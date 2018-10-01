//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/export_diagram.cpp
//! @brief     Implements class ExportDiagram
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/dialogs/export_diagram.h"
#include "core/session.h"
#include "gui/dialogs/subdialog_file.h"
#include "gui/mainwin.h"
//#include "qcr/base/debug.h"

//  ***********************************************************************************************
//! @class ExportDiagram

ExportDiagram::ExportDiagram()
    : QcrDialog(gGui, "Export diagram")
{
    fileField_ = new ExportfileDialogfield(this, true, [this]()->void{save();});

    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Export diagram data");

    setLayout(fileField_);
}

void ExportDiagram::save()
{
    QFile* file = fileField_->file();
    if (!file)
        return;
    QTextStream stream(file);

    QString separator = fileField_->separator();

    // get data
    const int idxX = int(gSession->params.diagramX.val());
    const int idxY = int(gSession->params.diagramY.val());
    std::vector<double> xs, ys, ysLow, ysHig;
    const InfoSequence* peakInfos = gSession->allPeaks.currentInfoSequence();
    ASSERT(peakInfos);
    peakInfos->get4(idxX, idxY, xs, ys, ysLow, ysHig);
    ASSERT(xs.size());
    // write data table
    for (int i=0; i<xs.size(); ++i) {
        stream << xs[i] << separator << ys[i];
        if (ysLow.size())
            stream << separator << ysLow[i] << separator << ysHig[i];
        stream << '\n';
    }

    close();
}
