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

#include "core/calc/export.h"
#include "gui/dialogs/export_bigtable.h"
#include "gui/mainwin.h"
#include "gui/view/bigtable.h"

namespace {

void writeBigtable(QTextStream& stream, const QString& separator)
{
    // get data
    QStringList headers {gGui->bigtableModel->getHeaders()};
    std::vector<std::vector<const QVariant*>> data {gGui->bigtableModel->getData()};

    // write header
    stream << "# ";
    for (const QString& header: headers)
        stream << header << separator;
    stream << '\n';

    // write data table
    for (const std::vector<const QVariant*>& row: data) {
        for (const QVariant* var: row) {
            if (var->canConvert<double>())
                stream << var->toDouble();
            else
                stream << var->toString();
            stream << separator;
        }
        stream << '\n';
    }
}

} // namespace


//  ***********************************************************************************************
//! @class ExportBigtable

ExportBigtable::ExportBigtable()
    : DialogSave{gGui, "ExportBigtable", "Big table export", data_export::defaultFormats}
{}

void ExportBigtable::writeCurrent(QTextStream& stream)
{
    writeBigtable(stream, data_export::separator(format()));
}
