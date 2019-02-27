//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/mixin.h
//! @brief     Defines classes QcrBaseMixin, QcrRootMixin, QcrRegisteredMixin,
//!                            QcrModalMixin, QcrModelessDialog
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

extern class QcrRootMixin* gRoot;

//! Mixin for QObject, enforcing a name, and providing recompute functionality.

//! All capture&replay enabled Qcr objects inherit from QcrBaseMixin.
//! All these Qcr objects also inherit from QObject.
//!
//! The order of this double inheritance is fixed:
//! Qcr objects must first inherit from a QObject or one of its children,
//! then from QcrBaseMixin or one of its children.
//! Only so the "object" argument of the QcrBaseMixin constructor can be set to "this".
//!
//! For example: @verbatim
//! class QcrComboBox : public QComboBox, public QcrSingleValue<int> { ... }    @endverbatim
//! inherits from QComboBox, which inherits from QObject,
//! and from QcrSingleValue<int>, which inherits from QcrBaseMixin.
//! The constructor is @verbatim
//!    QcrComboBox::QcrComboBox(const QString& _name, QcrCell<int>* _cell, ...)
//!        : QcrControl<int>{this, _name, _cell} {...}                          @endverbatim
//! where "this" points to a QComboBox, and is implicitly casted to QObject*.
//!
//! This mixin technology is a simpler alternative to diamond inheritance.
class QcrBaseMixin {
protected:
    QcrBaseMixin(QObject* object, const QString& name);
public:
    QcrBaseMixin() = delete;
    QcrBaseMixin(const QcrBaseMixin&) = delete;
    QcrBaseMixin operator=(const QcrBaseMixin&) = delete;
    //! Returns name of the associated QObject.
    const QString name() const { return object()->objectName(); }
    //! Calls the hook remake_, provided the associated QObject is a visible QWidget, or a QAction.
    virtual void remake();
    //! Sets the hook remake_ that is called from the function remake()
    void setRemake(std::function<void()> _remake) { remake_ = _remake; }
protected:
    //! Returns a pointer to the QObject that is associated with this mixin.
    const QObject* object() const { return object_; }
private:
    std::function<void()> remake_ {[](){}};
    QObject* object_;
};


//! Mixin for QMainWindow, inherited by QcrMainWindow, home of remakeAll().
class QcrRootMixin : public QcrBaseMixin {
public:
    QcrRootMixin(QObject* object);
    void pushDeletable(QObject* o) { deletables_.push_back(o); }
    void remakeAll();
private:
    std::vector<QObject*> deletables_;
};


//! Mix-in for QObject, enforcing a unique name, providing Console connection.
//!
//! Note on console learning and forgetting: In the c'tor, the console learns
//! the object name. Forgetting, however, must be implemented in a child class
//! destructor; otherwise it would happen too early.
class QcrRegisteredMixin : public QcrBaseMixin {
protected:
    QcrRegisteredMixin(QObject* object, const QString& name);
public:
    virtual void setFromCommand(const QString&) = 0;
protected:
    bool adhoc() const;
};


//! A modeless (= persistent spawned popup) dialog with support for capture&replay.
class QcrModelessDialog : protected QDialog, protected QcrRegisteredMixin {
protected:
    QcrModelessDialog(QWidget* parent, const QString& name);
    ~QcrModelessDialog();
public:
    void setFromCommand(const QString&) final;
private:
    void closeEvent(QCloseEvent*);
};

#endif // MIXIN_H
