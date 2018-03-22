// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/export_table.h
//! @brief     Defines class ExportTable
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef EXPORT_TABLE_H
#define EXPORT_TABLE_H

#include "frame.h"
#include <QWidget>

//! The modal dialog for saving the main table (fit results and metadata).

class ExportTable : private CModal, public QDialog {
public:
    ExportTable() = delete;
    ExportTable(bool xyMode);

private:
    class ExportfileDialogfield* fileField_;
    CRadioButton rbCurrent_       {"rbCurrent",       "Current diffractogram"};
    CRadioButton rbAllSequential_ {"rbAllSequential", "All diffractograms to numbered files"};
    CRadioButton rbAll_           {"rbAll",           "All diffractograms to one file"};

    void save();
};

#endif // EXPORT_TABLE_H
