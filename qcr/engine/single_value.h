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
#include "qcr/base/debug.h" // ASSERT
#include <QSettings>
#include <iostream>

//! Mixin for all QWidgets that hold a single value, base class for all QcrWidgets.
template<class T>
class QcrSingleValue : public QcrRegisteredMixin {
public:
    QcrSingleValue(QObject* object, const QString& name, QcrCell<T>* cell);
    QcrSingleValue(QObject* object, const QString& name, const T val);
    ~QcrSingleValue();
    //! Sets the value of the associated Cell, and in consequence also the value of this widget.
    void setCellValue(T val);
    //! Gets the current value of this widget, which agrees with the value of the associated cell.
    T getValue() const { ASSERT(doGetValue()==cell_->val()); return cell_->val(); }
    virtual void executeConsoleCommand(const QString& arg);
    QcrCell<T>* cell() { return cell_; }
    void setHook(std::function<void(const T)> f) { cell()->setHook(f); }
protected:
    void onChangedValue(T val);
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
QcrSingleValue<T>::QcrSingleValue(QObject* object, const QString& name, QcrCell<T>* cell)
    : QcrRegisteredMixin {object, name}
    , cell_ {cell}
{
    if (!adhoc()) {
        QSettings s;
        s.beginGroup("Controls");
        // Retrieve initial value from the config file controlled by QSettings
        QVariant v = s.value(QcrRegisteredMixin::name());
        if (v != QVariant{}) {
            const T val = v.value<T>();
            setCellValue(val);
            doLog(QcrRegisteredMixin::name()+" "+strOp::to_s(val));
        }
        // Value may have changed, therefore write back to the config file controlled by QSettings
        s.setValue(QcrRegisteredMixin::name(), cell_->val());
    }
    cell_->setCallbacks([this](){return doGetValue();}, [this](const T val){doSetValue(val);});
}

//! Constructs a QcrSingleValue that owns a QcrCell.
template<class T>
QcrSingleValue<T>::QcrSingleValue(QObject* object, const QString& name, const T val)
    : QcrRegisteredMixin {object, name}
    , ownsItsCell_ {true}
{
    cell_ = new QcrCell<T>(val); // TODO RECONSIDER smart pointer
    cell_->setCallbacks([this](){return doGetValue();}, [this](const T val){doSetValue(val);});
}

template<class T>
QcrSingleValue<T>::~QcrSingleValue()
{
    if (ownsItsCell_)
        delete cell_;
}

//! Sets the value of the associated Cell, and in consequence also the value of this widget.

//! This is the proper way of changing the widget's value programatically.

template<class T>
void QcrSingleValue<T>::setCellValue(T val)
{
    cell_->setVal(val);
}

template<class T>
void QcrSingleValue<T>::executeConsoleCommand(const QString& arg)
{
    doSetValue(strOp::from_s<T>(arg)); // unguarded
}

//! If value has changed, then logtransmit value to cell, and log.

//! Used by control widgets, typically through Qt signals that are emitted upon user actions.

template<class T>
void QcrSingleValue<T>::onChangedValue(T val)
{
    if (!adhoc()) {
        QSettings s;
        s.beginGroup("Controls");
        s.setValue(name(), val);
    }

    // qDebug()<<name()<<"onChangedValue arg="<<val<<", old="<<cell_->val();
    if (cell_->amCalling() || val==cell_->val())
        return;

    doLog(name()+" "+strOp::to_s(val));
    cell_->setVal(val);
    // qDebug()<<name()<<"remakeAll beg val="<<cell_->val();
    gRoot->remakeAll();
    // qDebug()<<name()<<"remakeAll end val="<<cell_->val();
}

#endif // SINGLE_VALUE_H
