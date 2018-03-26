// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/export_table.cpp
//! @brief     Implements class ExportTable
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "export_table.h"
#include "core/session.h"
#include "gui/base/file_dialog.h"
#include "gui/dialogs/exportfile_dialogfield.h"
#include "gui/mainwin.h"
#include "gui/panels/mainframe.h"
#include "gui/panels/data_table.h"
#include "gui/panels/tab_table.h"

// ************************************************************************** //
//  class ExportTable
// ************************************************************************** //

ExportTable::ExportTable(bool xyMode)
    : CModal("table")
    , QDialog(gGui)
    , xyMode_(xyMode)
{
    fileField_ = new ExportfileDialogfield(this, true, [this]()->void{save();});

    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(xyMode ? "Export diagram data" : "Export table");

    setLayout(fileField_);
}

void ExportTable::save()
{
    QString path = fileField_->filePath(true);
    if (path.isEmpty()) {
        qWarning() << "cannot save diagram: path is empty";
        return;
    }
    QFile* file = file_dialog::openFileConfirmOverwrite("file", this, path);
    if (!file)
        return;
    QTextStream stream(file);

    QString separator = fileField_->separator();
    if (xyMode_)
        ;//writeCurrentDiagramOutputFile(stream, separator);
    else
        gGui->mainframe().bigtableTab().dataView().toFile(stream, separator);
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
        const row_t& row = dataView_->row(i);
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
