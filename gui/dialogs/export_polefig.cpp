// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/export_polefig.cpp
//! @brief     Implements class ExportPolefig
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "export_polefig.h"
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
//  class ExportPolefig
// ************************************************************************** //

ExportPolefig::ExportPolefig()
    : CModal("dgram")
    , QDialog(gGui)
{
    rbAll_.setChecked(true);

    fileField_ = new ExportfileDialogfield(this, true, [this]()->void{save();});

    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Diffractograms");

    // layout
    auto* saveWhatLayout = new QVBoxLayout;
    saveWhatLayout->addWidget(&rbCurrent_);
    saveWhatLayout->addWidget(&rbAllSequential_);
    saveWhatLayout->addWidget(&rbAll_);

    auto* saveWhat = new QGroupBox {"Save what"};
    saveWhat->setLayout(saveWhatLayout);

    auto* vbox = new QVBoxLayout();
    vbox->addWidget(saveWhat);
    vbox->addLayout(fileField_);
    setLayout(vbox);

    show();
}

void ExportPolefig::save()
{
    close();
}
