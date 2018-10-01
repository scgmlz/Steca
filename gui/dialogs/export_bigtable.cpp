//  ***********************************************************************************************
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
//  ***********************************************************************************************

#include "gui/dialogs/export_bigtable.h"
#include "gui/dialogs/subdialog_file.h"
#include "gui/mainwin.h"
#include "gui/view/bigtable.h"

//  ***********************************************************************************************
//! @class ExportBigtable

ExportBigtable::ExportBigtable()
    : QcrDialog(gGui, "Export fit results table")
{
    fileField_ = new ExportfileDialogfield(this, true, [this]()->void{save();});

    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Export table");

    setLayout(fileField_);
}

// TODO move to Core
void ExportBigtable::save()
{
    QFile* file = fileField_->file();
    if (!file)
        return;
    QTextStream stream(file);

    QString separator = fileField_->separator();

    // get data
    QStringList headers {gGui->bigtableModel->getHeaders()};
    std::vector<std::vector<const QVariant*>> data {gGui->bigtableModel->getData()};

    // write header
    for (const QString& header: headers)
        stream << header << separator;
    stream << '\n';

    // write data table
    for (const std::vector<const QVariant*>& row: data) {
        for (const QVariant* var: row) {
            if (isNumeric(*var))
                stream << var->toDouble();
            else
                stream << var->toString();
            stream << separator;
        }
        stream << '\n';
    }

    close();
}
