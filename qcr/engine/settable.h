//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/settable.h
//! @brief     Defines classes QcrSettable, QcrModal, QcrModelessDialog
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#ifndef SETTABLE_H
#define SETTABLE_H

#include "qcr/engine/debug.h"
#include <QDialog>

//! Mix-in for control widgets that can be changed by a console command.
class QcrSettable {
public:
    virtual void executeConsoleCommand(const QString&) = 0;
    const QString& name() const { return name_; }
protected:
    QcrSettable() = delete;
    QcrSettable(const QcrSettable&) = delete;
    QcrSettable(const QString& name);
    ~QcrSettable();
    void doLog(bool softwareCalled, const QString& msg);
private:
    const QString name_;
};

//! Mix-in for modal dialogs.
class QcrModal {
protected:
    QcrModal() = delete;
    QcrModal(const QcrModal&) = delete;
    QcrModal(const QString& name);
    ~QcrModal();
};

//! A modeless (= persistent spawned popup) dialog with support for capture&replay.
class QcrModelessDialog : protected QDialog, protected QcrSettable {
public:
    void executeConsoleCommand(const QString&) final;
protected:
    QcrModelessDialog(QWidget* parent, const QString& name);
private:
    void closeEvent(QCloseEvent*);
};

#endif // SETTABLE_H
