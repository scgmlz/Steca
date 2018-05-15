//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/mixin.h
//! @brief     Defines classes QcrSettable, QcrModal, QcrModelessDialog
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#ifndef MIXIN_H
#define MIXIN_H

#include "qcr/base/debug.h"
#include <QDialog>

extern class QcrRoot* gRoot;

//! Mix-in for QObject, enforcing a name, and providing recompute functionality.
class QcrMixin {
public:
    const QObject& object() const { return object_; }
    const QString name() const { return object().objectName(); }
    virtual void remake();
protected:
    QcrMixin() = delete;
    QcrMixin(const QcrMixin&) = delete;
    QcrMixin(QObject& object, const QString& name);
private:
    QObject& object_;
};

//! Root of class hierarchy, normally mixed-in to QMainWindow
class QcrRoot : public QcrMixin {
public:
    QcrRoot(QObject& object, const QString& name);
    void fullRemake();
};


//! Mix-in for QObject, enforcing a unique name, providing Console connection.
class QcrSettable : public QcrMixin {
public:
    virtual void executeConsoleCommand(const QString&) = 0;
protected:
    QcrSettable(QObject& object, const QString& name);
    ~QcrSettable();
    void doLog(bool softwareCalled, const QString& msg);
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

#endif // MIXIN_H
