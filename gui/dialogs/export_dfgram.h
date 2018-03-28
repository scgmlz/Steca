// ************************************************************************************************
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
// ************************************************************************************************

#ifndef EXPORT_DFGRAM_H
#define EXPORT_DFGRAM_H

#include "gui/base/controls.h"

//! The modal dialog for saving diffractograms.

class ExportDfgram : private CModal, public QDialog {
public:
    ExportDfgram();

private:
    class ExportfileDialogfield* fileField_;
    CRadioButton rbCurrent_       {"rbCurrent",       "Current diffractogram"};
    CRadioButton rbAllSequential_ {"rbAllSequential", "All diffractograms to numbered files"};
    CRadioButton rbAll_           {"rbAll",           "All diffractograms to one file"};

    void save();
    void saveCurrent();
    void saveAll(bool oneFile);
};

#endif // EXPORT_DFGRAM_H
