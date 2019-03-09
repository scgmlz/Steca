//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/mixin.h
//! @brief     Defines classes QcrBase, QcrRoot, QcrRegistered, QcrModal, QcrModelessDialog
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

//!  for QObject, enforcing a name, and providing recompute functionality.

//! All capture&replay enabled Qcr objects inherit from QcrBase.
//! All these Qcr objects also inherit from QObject.
//!
//! The order of this double inheritance is fixed:
//! Qcr objects must first inherit from a QObject or one of its children,
//! then from QcrBase or one of its children.
//! Only so the "object" argument of the QcrBase constructor can be set to "this".
//!
//! For example: @verbatim
//! class QcrComboBox : public QComboBox, public QcrSingleValue<int> { ... }    @endverbatim
//! inherits from QComboBox, which inherits from QObject,
//! and from QcrSingleValue<int>, which inherits from QcrBase.
//! The constructor is @verbatim
//!    QcrComboBox::QcrComboBox(const QString& _name, QcrCell<int>* _cell, ...)
//!        : QcrControl<int>{this, _name, _cell} {...}                          @endverbatim
//! where "this" points to a QComboBox, and is implicitly casted to QObject*.
//!
//! This mixin technology is a simpler alternative to diamond inheritance.
class QcrBase {
protected:
    QcrBase(const QString& name);
public:
    QcrBase() = delete;
    QcrBase(const QcrBase&) = delete;
    //! Calls the hook remake_, provided the associated QObject is a visible QWidget, or a QAction.
    virtual void remake();
    //! Sets the hook remake_ that is called from the function remake()
    void setRemake(std::function<void()> _remake) { remake_ = _remake; }
    QString name() const { return name_; }
private:
    std::function<void()> remake_ {[](){}};
    const QString name_;
};


//! Mixin for QMainWindow, inherited by QcrMainWindow, home of remakeAll().
class QcrRoot : public QcrBase {
public:
    QcrRoot();
    void remakeAll();
};

class QcrCommandable : public QcrBase {
protected:
    QcrCommandable(const QString& name);
public:
    virtual void setFromCommand(const QString&) = 0;
protected:
    bool adhoc() const;
};


//! Mix-in for QObject, enforcing a unique name, providing Console connection.

//! Note on console learning and forgetting: In the c'tor, the console learns
//! the object name. Forgetting, however, must be implemented in a child class
//! destructor; otherwise it would happen too early.
class QcrRegistered : public QcrCommandable {
protected:
    QcrRegistered(const QString& name);
    ~QcrRegistered();
};


//! A modeless (= persistent spawned popup) dialog with support for capture&replay.
class QcrModelessDialog : protected QcrRegistered, protected QDialog {
protected:
    QcrModelessDialog(QWidget* parent, const QString& name);
    ~QcrModelessDialog();
public:
    void setFromCommand(const QString&) final;
private:
    void closeEvent(QCloseEvent*);
};

#endif // MIXIN_H
