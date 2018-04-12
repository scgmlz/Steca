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

#include "gui/base/controls.h"

//! The modal dialog for saving the main polefig (fit results and metadata).

class ExportPolefig : private CModal, public QDialog {
public:
    ExportPolefig();

private:
    class ExportfileDialogfield* fileField_;
    CRadioButton rbCurrent_       {"rbCurrent",       "Current peak only"};
    CRadioButton rbAllSequential_ {"rbAllSequential", "All peaks to numbered files"};
    CRadioButton rbAll_           {"rbAll",           "All peaks to one file"};
    CRadioButton rbOriginalGrid_  {"rbOriginalGrid",  "Original α-β grid"};
    CRadioButton rbInterpolated_  {"rbInterpolated",  "Interpolated α-β grid"};

    bool interpolated();
    void save();
    void saveCurrent();
    void saveAll(bool oneFile);
};

#endif // EXPORT_POLEFIG_H
