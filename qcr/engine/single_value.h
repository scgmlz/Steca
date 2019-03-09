//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/single_value.h
//! @brief     Defines and implements templated classes QcrSingleValue
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#ifndef SINGLE_VALUE_H
#define SINGLE_VALUE_H

#include "qcr/base/string_ops.h"
#include "qcr/engine/mixin.h"
#include "qcr/engine/cell.h"
#include "qcr/engine/console.h"
#include "qcr/base/debug.h" // ASSERT
#include <QSettings>
#include <iostream>

//!  for all QWidgets that hold a single value, base class for all QcrWidgets.
template<class T>
class QcrSingleValue : public QcrRegistered {
public:
    QcrSingleValue(const QString& name, QcrCell<T>* cell);
    QcrSingleValue(const QString& name, const T val);
    ~QcrSingleValue();
    //! Sets the value of the associated Cell, and in consequence also the value of this widget.
    void setCellValue(T val);
    //! Sets the widget value according to string argument. Called by Console::wrappedCommand.
    virtual void setFromCommand(const QString& arg);
    //! Gets the current value of this widget, which agrees with the value of the associated cell.
    T getValue() const { ASSERT(doGetValue()==cell_->val()); return cell_->val(); }
    //! Returns pointer to associated Cell.
    QcrCell<T>* cell() { return cell_; }
    //! Sets the hook of the associated Cell.
    void setHook(std::function<void(const T)> f) { cell()->setHook(f); }
protected:
    //! Transmits new widget value to the Cell, to the QSettings. Logs. Finally remakes all views.
    void onChangedValue(T val, const QString& comment="");
    QcrCell<T>* cell_ {nullptr};
private:
    virtual T doGetValue() const = 0; //!< to be overridden by the widget-specific get function
    virtual void doSetValue(T) = 0;   //!< to be overridden by the widget-specific set function
    bool ownsItsCell_ {false};
};

//  ***********************************************************************************************
//  implementation of QcrSingleValue<T>

//! Constructor that associates this QcrSingleValue with an external QcrCell.
template<class T>
QcrSingleValue<T>::QcrSingleValue(const QString& name, QcrCell<T>* cell)
    : QcrRegistered{name}
    , cell_ {cell}
{
    if (!adhoc()) {
        QSettings s;
        s.beginGroup("Controls");
        // Retrieve initial value from the config file controlled by QSettings
        QVariant v = s.value(QcrRegistered::name());
        if (v != QVariant{}) {
            const T val = v.value<T>();
            setCellValue(val);
            gConsole->log(QcrRegistered::name()+" "+strOp::to_s(val));
        }
        // Value may have changed, therefore write back to the config file controlled by QSettings
        s.setValue(QcrRegistered::name(), cell_->val());
    }
    cell_->setCallbacks([this](){return doGetValue();}, [this](const T val){doSetValue(val);});
}

//! Constructs a QcrSingleValue that owns a QcrCell.
template<class T>
QcrSingleValue<T>::QcrSingleValue(const QString& name, const T val)
    : QcrRegistered{name}
    , ownsItsCell_ {true}
{
    cell_ = new QcrCell<T>{val}; // TODO RECONSIDER smart pointer
    cell_->setCallbacks([this](){return doGetValue();}, [this](const T val){doSetValue(val);});
}

template<class T>
QcrSingleValue<T>::~QcrSingleValue()
{
    QTextStream qterr(stderr);
    if (ownsItsCell_)
        delete cell_;
    else
        cell_->releaseCallbacks();
}

//! Sets the value of the associated Cell, and in consequence also the value of this widget.

//! This is the proper way of changing the widget's value programatically.
template<class T>
void QcrSingleValue<T>::setCellValue(T val)
{
    cell_->setVal(val);
}

//! Sets the widget value according to string argument. Called by Console::wrappedCommand.
template<class T>
void QcrSingleValue<T>::setFromCommand(const QString& arg)
{
    doSetValue(strOp::from_s<T>(arg)); // unguarded
}

//! Transmits new widget value to the Cell, to the QSettings. Logs. Finally remakes all views.

//! Used by control widgets, typically through Qt signals that are emitted upon user actions.
//! Optionally, a comment is appended to the log entry.
template<class T>
void QcrSingleValue<T>::onChangedValue(T val, const QString& comment)
{
    if (!adhoc()) {
        QSettings s;
        s.beginGroup("Controls");
        s.setValue(name(), val);
    }
    if (cell_->amCalling() || val==cell_->val())
        return;
    QString line = name()+" "+strOp::to_s(val);
    if (comment!="")
        line += " # " + comment;
    gConsole->log(line);
    cell_->setVal(val);
    gRoot->remakeAll();
}

#endif // SINGLE_VALUE_H
