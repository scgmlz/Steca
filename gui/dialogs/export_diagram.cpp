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
#include "gui/dialogs/subdialog_file.h"
#include "gui/mainwin.h"
//#include "qcr/base/debug.h"

//  ***********************************************************************************************
//! @class ExportDiagram

ExportDiagram::ExportDiagram()
    : QcrDialog(gGui, "Export diagram")
{
    fileField_ = new DialogfieldFile(
        this, data_export::defaultFormats, data_export::saveDiagram);

    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Export diagram data");

    setLayout(fileField_);
}
