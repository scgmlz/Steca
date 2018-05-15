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

#include "export_diagram.h"
#include "core/session.h"
#include "core/def/idiomatic_for.h"
#include "gui/dialogs/exportfile_dialogfield.h"
#include "gui/mainwin.h"
#include "gui/state.h"
#include "qcr/engine/debug.h"

//  ***********************************************************************************************
//! @class ExportDiagram

ExportDiagram::ExportDiagram()
    : QcrModal("xdia")
    , QDialog(gGui)
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
    const int xi = int(gGui->state->diagramX->getValue());
    const int yi = int(gGui->state->diagramY->getValue());
    std::vector<double> xs, ys, ysLow, ysHig;
    gSession->peakInfos().get4(xi, yi, xs, ys, ysLow, ysHig);
    if (!xs.size()) {
        qWarning() << "no data available";
        return;
    }

    // write data table
    for_i (xs.size()) {
        stream << xs[i] << separator << ys[i];
        if (ysLow.size())
            stream << separator << ysLow[i] << separator << ysHig[i];
        stream << '\n';
    }

    close();
}
