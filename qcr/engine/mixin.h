//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/mixin.h
//! @brief     Defines classes QcrMixin, QcrRoot, QcrSettable, QcrModal, QcrModelessDialog
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#ifndef MIXIN_H
#define MIXIN_H

//#include "qcr/base/debug.h"
#include <QDialog>
#include <functional>

extern class QcrRoot* gRoot;

//! Mix-in for QObject, enforcing a name, and providing recompute functionality.
class QcrMixin {
protected:
    QcrMixin(QObject& object, const QString& name); // TODO convert everywhere to QObject* form
    QcrMixin(QObject* object, const QString& name="");
public:
    QcrMixin(const QcrMixin&) = delete;
    const QObject& object() const { return object_; }
    const QString name() const { return object().objectName(); }
    virtual void remake();
    void setRemake(std::function<void()> _remake) { remake_ = _remake; }
private:
    std::function<void()> remake_ {[](){}};
    QObject& object_;
};


//! Root of class hierarchy, for inheritance by QcrMainWindow.
class QcrRoot : public QcrMixin {
public:
    QcrRoot(QObject* object);
    void remakeAll();
};


//! Mix-in for QObject, enforcing a unique name, providing Console connection.
class QcrSettable : public QcrMixin {
protected:
    QcrSettable(QObject& object, const QString& name, bool _modal=false);
public:
    virtual void executeConsoleCommand(const QString&) = 0;
protected:
    void doLog(const QString& msg);
};


//! A modeless (= persistent spawned popup) dialog with support for capture&replay.
class QcrModelessDialog : protected QDialog, protected QcrSettable {
protected:
    QcrModelessDialog(QWidget* parent, const QString& name);
public:
    void executeConsoleCommand(const QString&) final;
private:
    void closeEvent(QCloseEvent*);
};

#endif // MIXIN_H
