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
#include "qcr/widgets/modal_dialogs.h"
#include <QButtonGroup>

class InfoSequence;

//! The modal dialog for saving the main polefig (fit results and metadata).

class ExportPolefig : public QcrDialog {
public:
    ExportPolefig();

private:
    class ExportfileDialogfield* fileField_;
    QcrRadioButton exportCurrent_ {"exportCurrent", "Current peak only"};
    QcrRadioButton exportMulti_   {"exportMulti",   "All peaks to numbered files"};
    QcrRadioButton exportCombi_   {"exportCombi",   "All peaks to one file"};
    QButtonGroup exportModeGroup;

    void save();
    //void saveCurrent();
    void saveAll(bool oneFile, const QString &path, const QString &separator,
                 const std::vector<InfoSequence const *> &peaks);

    void executeConsoleCommand(const QString&) override {}

    enum class ExportMode {
        CURRENT_PEAK = 0,
        ALL_PEAKS_MULTIPLE_FILES = 1,
        ALL_PEAKS_SINGLE_FILE = 2,
    };
};

#endif // EXPORT_POLEFIG_H
