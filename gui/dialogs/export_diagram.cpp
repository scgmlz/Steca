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
#include "core/data/export.h"
#include "core/session.h"
#include "gui/mainwin.h"
//#include "qcr/base/debug.h"

//  ***********************************************************************************************
//! @class ExportDiagram

ExportDiagram::ExportDiagram()
    : DialogSave(gGui, "ExportDiagram", "Diagram data export", data_export::defaultFormats)
{
}

void ExportDiagram::writeCurrent(QTextStream&)
{
}
