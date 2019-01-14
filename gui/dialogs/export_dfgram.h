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

#include "qcr/widgets/controls.h"
#include "qcr/widgets/modal_dialogs.h"
#include "QButtonGroup"

//! The modal dialog for saving diffractograms.

class ExportDfgram : public QcrDialog {
public:
    ExportDfgram();

private:
    class ExportfileDialogfield* fileField_;
    QcrRadioButton rbCurrent_       {"rbCurrent",       "Current diffractogram"};
    QcrRadioButton rbAllSequential_ {"rbAllSequential", "All diffractograms to numbered files"};
    QcrRadioButton rbAll_           {"rbAll",           "All diffractograms to one file"};

    static void save(QFile* file, const QString& format, QcrDialog* parent);
    static void saveCurrent(QFile* file, const QString& format);
    static void saveAll(QFile* file, const QString& format, ExportDfgram* parent);

    bool modeCurrent() const { return rbCurrent_.getValue(); }
    bool modeSequential() const { return !modeCurrent() && rbAllSequential_.getValue(); }
};

#endif // EXPORT_DFGRAM_H
