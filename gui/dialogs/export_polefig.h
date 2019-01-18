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

#include "gui/dialogs/dialog_save.h"
#include <QButtonGroup>

class InfoSequence;

//! The modal dialog for saving the main polefig (fit results and metadata).

class ExportPolefig : public DialogMultisave {
public:
    ExportPolefig();
private:
    void writeCurrent(QTextStream&) final;
};

#endif // EXPORT_POLEFIG_H
