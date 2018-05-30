//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/single_value.h
//! @brief     Defines and implements templated classes QcrControl
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#ifndef SINGLE_VALUE_H
#define SINGLE_VALUE_H

#include "qcr/engine/mixin.h"
#include "qcr/engine/cell.h"
#include "qcr/base/string_ops.h"

//! Base class for all Qcr widgets that hold a single value.
template<class T>
class QcrControl : public QcrSettable {
public:
    QcrControl(QObject& object, const QString& name, SingleValueCell<T>* cell);
    QcrControl(QObject& object, const QString& name, const T val);
    ~QcrControl();
    void programaticallySetValue(T val);
    virtual T getValue() const = 0;
    virtual void executeConsoleCommand(const QString& arg);
    Cell* cell() { return cell_; }
protected:
    void init();
    void onChangedValue(bool hasFocus, T val);
    bool softwareCalling_ = false; // make it private again ??
    SingleValueCell<T>* cell_ {nullptr};
private:
    virtual void doSetValue(T) = 0;
    T reportedValue_;
    bool ownsItsCell_ {false};
};

//  ***********************************************************************************************
//  implementation of QcrControl<T>

//! Constructor that associates this QcrControl with an external SingleValueCell.
template<class T>
QcrControl<T>::QcrControl(QObject& object, const QString& name, SingleValueCell<T>* cell)
    : QcrSettable {object, name}
    , cell_ {cell}
{
    cell_->connectAction([this](){programaticallySetValue(cell_->val());});
}

//! Constructs a QcrControl that owns a SingleValueCell.
template<class T>
QcrControl<T>::QcrControl(QObject& object, const QString& name, const T val)
    : QcrSettable {object, name}
    , ownsItsCell_ {true}
{
    // TODO RECONSIDER smart pointer
    cell_ = new SingleValueCell<T>(name, val);
    cell_->connectAction([this](){programaticallySetValue(cell_->val());});
}

template<class T>
QcrControl<T>::~QcrControl()
{
    if (ownsItsCell_)
        delete cell_;
}

template<class T>
void QcrControl<T>::programaticallySetValue(T val)
{
    softwareCalling_ = true;
    doSetValue(val);
    softwareCalling_ = false;
}

template<class T>
void QcrControl<T>::executeConsoleCommand(const QString& arg)
{
    programaticallySetValue(strOp::from_s<T>(arg));
}

template<class T>
void QcrControl<T>::init()
{
    reportedValue_ = getValue();
    doLog(true, "initialization: "+name()+" "+strOp::to_s(reportedValue_));
}

template<class T>
void QcrControl<T>::onChangedValue(bool hasFocus, T val)
{
    if (val==reportedValue_)
        return; // nothing to do
    bool userCall = hasFocus || !softwareCalling_;
    doLog(!userCall, name()+" "+strOp::to_s(val));

    // not sure whether we want to get rid of hasFocus; perform some tests:
    if (hasFocus && softwareCalling_)
        qDebug() << "UNEXPECTED in "+name()+" hasFocus && softwareCalling_";
    //if (!hasFocus && !softwareCalling_)
    //    qDebug() << "UNDESIRABLE in "+name()+" !hasFocus && !softwareCalling_";
    // <= seems unavoidable in QTabWidget tab selection

    reportedValue_ = val;
    if (cell_)
        cell_->setVal(val, userCall);
}

#endif // SINGLE_VALUE_H
