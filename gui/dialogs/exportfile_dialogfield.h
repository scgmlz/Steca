// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/exportfile_dialogfield.h
//! @brief     Defines class ExportfileDialogfield
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef EXPORTFILE_DIALOGFIELD_H
#define EXPORTFILE_DIALOGFIELD_H

#include "gui/base/controls.h"
#include <QProgressBar>

//! Base class for dialogs for saving some output to a file.

class ExportfileDialogfield : public QVBoxLayout {
public:
    ExportfileDialogfield() = delete;
    ExportfileDialogfield(ExportfileDialogfield&) = delete;
    ExportfileDialogfield(QWidget* parent, bool withTypes, std::function<void(void)> onSave);

    QString filePath(bool withSuffix, bool withNumber=false);
    QString separator() const;
    QProgressBar progressBar;
private:
    QLineEdit* dir_;
    QLineEdit* file_;
};

#endif // EXPORTFILE_DIALOGFIELD_H
