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
#include "core/calc/export.h"
#include "gui/mainwin.h"

ExportDiagram::ExportDiagram()
    : DialogSave{gGui, "ExportDiagram", "Diagram data export", data_export::defaultFormats}
{
}

void ExportDiagram::writeCurrent(QTextStream& stream)
{
    data_export::writeDiagram(stream, data_export::separator(format()));
}
