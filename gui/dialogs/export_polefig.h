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

#include "qcr/widgets/controls.h"

//! The modal dialog for saving the main polefig (fit results and metadata).

class ExportPolefig : public QcrDialog {
public:
    ExportPolefig();

private:
    class ExportfileDialogfield* fileField_;
    QcrRadioButton rbCurrent_       {"rbCurrent",       "Current peak only"};
    QcrRadioButton rbAllSequential_ {"rbAllSequential", "All peaks to numbered files"};
    QcrRadioButton rbAll_           {"rbAll",           "All peaks to one file"};
    QcrRadioButton rbOriginalGrid_  {"rbOriginalGrid",  "Original α-β grid"};
    QcrRadioButton rbInterpolated_  {"rbInterpolated",  "Interpolated α-β grid"};

    bool interpolated();
    void save();
    void saveCurrent();
    void saveAll(bool oneFile);

    void onCommand(const QString&) override {}
};

#endif // EXPORT_POLEFIG_H
