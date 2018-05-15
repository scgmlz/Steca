//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/settable.h
//! @brief     Defines classes CSettable, CModal, CModelessDialog
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
class CSettable {
public:
    virtual void executeConsoleCommand(const QString&) = 0;
    const QString& name() const { return name_; }
protected:
    CSettable() = delete;
    CSettable(const CSettable&) = delete;
    CSettable(const QString& name);
    ~CSettable();
    void doLog(bool softwareCalled, const QString& msg);
private:
    const QString name_;
};

//! Mix-in for modal dialogs.
class CModal {
protected:
    CModal() = delete;
    CModal(const CModal&) = delete;
    CModal(const QString& name);
    ~CModal();
};

//! A modeless (= persistent spawned popup) dialog with support for capture&replay.
class CModelessDialog : protected QDialog, protected CSettable {
public:
    void executeConsoleCommand(const QString&) final;
protected:
    CModelessDialog(QWidget* parent, const QString& name);
private:
    void closeEvent(QCloseEvent*);
};

#endif // SETTABLE_H
