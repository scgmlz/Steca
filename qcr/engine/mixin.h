//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/mixin.h
//! @brief     Defines classes QcrBaseMixin, QcrRootMixin, QcrRegisteredMixin, QcrModalMixin, QcrModelessDialog
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

namespace Qcr {
extern bool replay;
}

extern class QcrRootMixin* gRoot;

//! Mix-in for QObject, enforcing a name, and providing recompute functionality.
class QcrBaseMixin {
protected:
    QcrBaseMixin(QObject* object, const QString& name);
public:
    QcrBaseMixin(const QcrBaseMixin&) = delete;
    QcrBaseMixin operator=(const QcrBaseMixin&) = delete;
    const QString name() const { return object()->objectName(); }
    virtual void remake();
    void setRemake(std::function<void()> _remake) { remake_ = _remake; }
protected:
    const QObject* object() const { return object_; }
private:
    std::function<void()> remake_ {[](){}};
    QObject* object_;
};


//! Root of class hierarchy, for inheritance by QcrMainWindow.
class QcrRootMixin : public QcrBaseMixin {
public:
    QcrRootMixin(QObject* object);
    void remakeAll();
};


//! Mix-in for QObject, enforcing a unique name, providing Console connection.
class QcrRegisteredMixin : public QcrBaseMixin {
protected:
    QcrRegisteredMixin(QObject* object, const QString& name, bool _modal=false);
public:
    virtual void executeConsoleCommand(const QString&) = 0;
protected:
    void doLog(const QString& msg);
    bool adhoc() const;
};


//! A modeless (= persistent spawned popup) dialog with support for capture&replay.
class QcrModelessDialog : protected QDialog, protected QcrRegisteredMixin {
protected:
    QcrModelessDialog(QWidget* parent, const QString& name);
public:
    void executeConsoleCommand(const QString&) final;
private:
    void closeEvent(QCloseEvent*);
};

#endif // MIXIN_H
