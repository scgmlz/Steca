//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/subdialog_file.h
//! @brief     Defines class ExportfileDialogfield
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef SUBDIALOG_FILE_H
#define SUBDIALOG_FILE_H

#include "qcr/widgets/controls.h"
#include "qcr/widgets/modal_dialogs.h"
#include <QProgressBar>
#include <QButtonGroup>

//! Returns templatedName with '%d' replaced by string representation of num.

//!  The string representation of num has leading zeros, and its number of
//!  digits is determined by the maximum value maxNum.

QString numberedFileName(const QString& templatedName, int num, int maxNum);

//! Base class for dialogs for saving some output to a file.

class ExportfileDialogfield : public QVBoxLayout {
public:
    ExportfileDialogfield() = delete;
    ExportfileDialogfield(const ExportfileDialogfield&) = delete;
    ExportfileDialogfield(QWidget* parent, bool withTypes, std::function<void(void)> onSave);

    QString path(bool withSuffix, bool withNumber=false);
    QFile* file();
    QString separator() const;
    QProgressBar progressBar;
private:
    QLineEdit* dir_;
    QLineEdit* file_;
    QcrRadioButton rbDat_ {"fmtDat", ".dat"};
    QcrRadioButton rbCsv_ {"fmtCsv", ".csv"};
    QButtonGroup fileExtensionGroup;
    QcrTrigger actBrowse_ {"selectDir", "Browse..."};
    QcrTrigger actCancel_ {"cancel", "Cancel"};
    QcrTrigger actSave_ {"save", "Save"};
};

#endif // SUBDIALOG_FILE_H
