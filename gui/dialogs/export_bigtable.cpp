// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/export_bigtable.cpp
//! @brief     Implements class ExportBigtable
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "export_bigtable.h"
#include "core/session.h"
#include "gui/base/file_dialog.h"
#include "gui/dialogs/exportfile_dialogfield.h"
#include "gui/mainwin.h"
#include "gui/state.h"
#include "gui/view/bigtable.h"

// ************************************************************************** //
//  class ExportBigtable
// ************************************************************************** //

ExportBigtable::ExportBigtable(bool xyMode)
    : CModal("exportTable")
    , QDialog(gGui)
    , xyMode_(xyMode)
{
    fileField_ = new ExportfileDialogfield(this, true, [this]()->void{save();});

    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(xyMode ? "Export diagram data" : "Export table");

    setLayout(fileField_);
}

void ExportBigtable::save()
{
    QFile* file = fileField_->file();
    if (!file)
        return;
    QTextStream stream(file);

    QString separator = fileField_->separator();
    if (xyMode_)
        ;//writeCurrentDiagramOutputFile(stream, separator);
    else
        gGui->state->bigtableModel->toFile(stream, separator);
    close();
}


/*
void DiagramsFrame::writeCurrentDiagramOutputFile(QTextStream& stream, const QString& separator)
{
    ASSERT(xs_.count() == ys_.count());
    ASSERT(ysErrorLo_.isEmpty() || ysErrorLo_.count() == ys_.count());
    ASSERT(ysErrorLo_.count() == ysErrorUp_.count());

    bool writeErrors = !ysErrorUp_.isEmpty();

    for_i (xs_.count()) {
        stream << xs_.at(i) << separator << ys_.at(i);
        if (writeErrors)
            stream << separator << ysErrorLo_.at(i) << separator << ysErrorUp_.at(i);
        stream << '\n';
    }
}

void DiagramsFrame::writeAllDataOutputFile(QTextStream& stream, const QString& separator)
{
    const QStringList& headers = dataView_->outHeaders();
    for_i (headers.count())
        stream << headers.at(i) << separator;
    stream << '\n';

    for_i (calcPoints_.at(getReflIndex()).count()) {
        const QVector<QVariant>& row = dataView_->row(i);
        for_i (row.count()) {
            const QVariant& var = row.at(i);
            if (isNumeric(var))
                stream << var.toDouble();
            else
                stream << var.toString();
            stream << separator;
        }
        stream << '\n';
    }
}
*/
