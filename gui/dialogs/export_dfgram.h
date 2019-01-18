//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/export_dfgram.h
//! @brief     Defines class ExportDfgram
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef EXPORT_DFGRAM_H
#define EXPORT_DFGRAM_H

#include "gui/dialogs/dialog_save.h"

//! The modal dialog for saving diffractograms.

class ExportDfgram : public DialogMultisave {
public:
    ExportDfgram();
private:
    void writeCurrent(QTextStream&) final;
    int multiplicity() final;
    void writeJointfile(QTextStream&) final;
};

#endif // EXPORT_DFGRAM_H
