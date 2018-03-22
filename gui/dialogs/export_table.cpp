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
#include <cmath>
#include <QGroupBox>
#include <QMessageBox>

namespace {

// TODO move file saving code to Core

} // namespace


// ************************************************************************** //
//  class ExportTable
// ************************************************************************** //

ExportTable::ExportTable(bool xyMode)
    : CModal("dgram")
    , QDialog(gGui)
{
    fileField_ = new ExportfileDialogfield(this, true, [this]()->void{save();});

    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Diffractograms");

    setLayout(fileField_);
}

void ExportTable::save()
{
    close();
}
