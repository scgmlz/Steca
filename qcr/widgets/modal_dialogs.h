//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/widgets/modal_dialogs.h
//! @brief     Defines functions that return new Qt objects
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#ifndef DIALOGS_H
#define DIALOGS_H

#include "qcr/engine/mixin.h"
#include <QFileDialog>
#include <QtGlobal>

//! Mix-in for modal dialogs.
class QcrModal : public QcrSettable {
protected:
    QcrModal(QObject& object, const QString& name);
    ~QcrModal();
};

//! Dialog, for modal use.
class QcrDialog : public QDialog, protected QcrModal {
public:
    QcrDialog(QWidget* parent, const QString& caption);
    ~QcrDialog();
    int exec() override;
    void executeConsoleCommand(const QString&) override;
};

//! File dialog, for modal use, with console commands to select files and to close the dialog.
class QcrFileDialog : public QFileDialog, protected QcrModal {
public:
    QcrFileDialog(
        QWidget* parent, const QString& caption, const QString& directory, const QString& filter);
    ~QcrFileDialog();
    int exec() override;
    void executeConsoleCommand(const QString&) override;
};

#endif // DIALOGS_H
