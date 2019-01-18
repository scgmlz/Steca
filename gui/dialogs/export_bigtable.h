//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/export_bigtable.h
//! @brief     Defines class ExportBigtable
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef EXPORT_BIGTABLE_H
#define EXPORT_BIGTABLE_H

#include "gui/dialogs/dialog_save.h"

//! The modal dialog for saving the main table (fit results and metadata).

class ExportBigtable : public DialogSave {
public:
    ExportBigtable();
private:
    void writeCurrent(QTextStream&) final;
};

#endif // EXPORT_BIGTABLE_H
