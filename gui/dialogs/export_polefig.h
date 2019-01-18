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
    class DialogMultisave* fileField_;
    QcrRadioButton exportCurrent_ {"exportCurrent", "Current peak only"};
    QcrRadioButton exportMulti_   {"exportMulti",   "All peaks to numbered files"};
    QcrRadioButton exportCombi_   {"exportCombi",   "All peaks to one file"};
    QButtonGroup exportModeGroup; // not privatizable yet

    static void save(QFile*, const QString& format, QcrDialog* parent);
    //void saveCurrent();
    static void saveAll(bool oneFile, const QString& path, const QString& separator,
                 const std::vector<const InfoSequence*>& peaks, QcrDialog* parent);

    void executeConsoleCommand(const QString&) override {}
};

#endif // EXPORT_POLEFIG_H
