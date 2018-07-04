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

#include "qcr/base/debug.h"
#include "qcr/base/string_ops.h"
#include "qcr/engine/mixin.h"
#include "qcr/engine/param_wrapper.h"

//! Base class for all Qcr widgets that hold a single value.
template<class T>
class QcrControl : public QcrSettable {
public:
    QcrControl(QObject& object, const QString& name, ParamWrapper<T>* cell);
    QcrControl(QObject& object, const QString& name, const T val);
    ~QcrControl();
    void programaticallySetValue(T val);
    virtual T getValue() const = 0;
    virtual void executeConsoleCommand(const QString& arg);
    ParamWrapper<T>* cell() { return cell_; }
protected:
    void initControl();
    void onChangedValue(bool hasFocus, T val);
    bool softwareCalling_ {false}; // make it private again ??
    ParamWrapper<T>* cell_ {nullptr};
private:
    virtual void doSetValue(T) = 0;
    T reportedValue_;
    bool ownsItsCell_ {false};
};

//  ***********************************************************************************************
//  implementation of QcrControl<T>

//! Constructor that associates this QcrControl with an external ParamWrapper.
template<class T>
QcrControl<T>::QcrControl(QObject& object, const QString& name, ParamWrapper<T>* cell)
    : QcrSettable {object, name}
    , cell_ {cell}
{
}

//! Constructs a QcrControl that owns a ParamWrapper.
template<class T>
QcrControl<T>::QcrControl(QObject& object, const QString& name, const T val)
    : QcrSettable {object, name}
    , ownsItsCell_ {true}
{
    cell_ = new ParamWrapper<T>(val); // TODO RECONSIDER smart pointer
}

template<class T>
QcrControl<T>::~QcrControl()
{
    if (ownsItsCell_)
        delete cell_;
}

//! Ensures synchronization of this Control with its associated Cell.

//! Cannot be called from QcrControl constructors, because it calls the pure virtual member
//! function 'doSetValue'; its overrides are not available in the constructor of 'QcrControl'.
template<class T>
void QcrControl<T>::initControl()
{
    T givenValue = cell_->val();
    programaticallySetValue(givenValue);
    reportedValue_ = getValue();
    if (!ownsItsCell_ && reportedValue_ != givenValue)
        qWarning("Widget %s has changed value of cell from %s to %s",
               name().toLatin1().constData(),
               strOp::to_s(givenValue).toLatin1().constData(),
               strOp::to_s(reportedValue_).toLatin1().constData());
    cell_->setGuiVal_ = [this](T val){programaticallySetValue(val);};
    doLog(true, "initControl "+name()+" "+strOp::to_s(reportedValue_));
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
    cell_->guiSetsVal(val, userCall);
}

#endif // SINGLE_VALUE_H
