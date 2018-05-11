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

#include "qcr/engine/settable.h"
#include "qcr/engine/string_ops.h"

//! Base class for all Qcr widgets that hold a single value.
template<class T>
class QcrControl : protected CSettable {
public:
    QcrControl(const QString& name);
    void programaticallySetValue(T val);
    virtual T getValue() const = 0;
    virtual void onCommand(const QString& arg);
protected:
    void init();
    void onChangedValue(bool hasFocus, T val);
    bool softwareCalling_ = false; // make it private again ??
private:
    virtual void doSetValue(T) = 0;
    T reportedValue_;
};

//  ***********************************************************************************************
//  implementation of QcrControl<T>

template<class T>
QcrControl<T>::QcrControl(const QString& name)
    : CSettable(name)
{}

template<class T>
void QcrControl<T>::programaticallySetValue(T val)
{
    softwareCalling_ = true;
    doSetValue(val);
    softwareCalling_ = false;
}

template<class T>
void QcrControl<T>::onCommand(const QString& arg)
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
    bool userCall = hasFocus && !softwareCalling_;
    doLog(!userCall, name()+" "+strOp::to_s(val));
    if (hasFocus && softwareCalling_)
        qDebug() << "UNEXPECTED in "+name()+" hasFocus && softwareCalling_";
    if (!hasFocus && !softwareCalling_)
        qDebug() << "UNDESIRABLE in "+name()+" !hasFocus && !softwareCalling_";
    // not sure whether we want to get rid of hasFocus
    reportedValue_ = val;
}

#endif // SINGLE_VALUE_H
