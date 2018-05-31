//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/export_polefig.h
//! @brief     Defines class ExportPolefig
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef EXPORT_POLEFIG_H
#define EXPORT_POLEFIG_H

#include "qcr/widgets/modal_dialogs.h"
#include "qcr/widgets/controls.h"

//! The modal dialog for saving the main polefig (fit results and metadata).

class ExportPolefig : public QcrDialog {
public:
    ExportPolefig();

private:
    class ExportfileDialogfield* fileField_;
    QcrRadioButton exportCurrent_ {"exportCurrent", "Current peak only"};
    QcrRadioButton exportMulti_   {"exportMulti",   "All peaks to numbered files"};
    QcrRadioButton exportCombi_   {"exportCombi",   "All peaks to one file"};
    QcrRadioButton gridOriginal_  {"gridOriginal",  "Original α-β grid"};
    QcrRadioButton gridInterpol_  {"gridInterpol",  "Interpolated α-β grid"};

    bool interpolated();
    void save();
    void saveCurrent();
    void saveAll(bool oneFile);

    void executeConsoleCommand(const QString&) override {}
};

#endif // EXPORT_POLEFIG_H
