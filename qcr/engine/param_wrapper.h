//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/param_wrapper.h
//! @brief     Defines and implements templated class ParamWrapper
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#ifndef PARAM_WRAPPER_H
#define PARAM_WRAPPER_H

#include "qcr/base/debug.h"
#include "qcr/base/string_ops.h"
#include "qcr/engine/mixin.h"
#include <QObject>
#include <functional>
#include <vector>

template<class T>
class QcrControl;

//! Holds a single parameter.
template<class T>
class ParamWrapper {
public:
    ParamWrapper() = delete;
    ParamWrapper(T value) : value_{value} {}

    void setVal(T);
    T val() const { return value_; }

    void setHook(std::function<void(T)> hook) { hook_ = hook; }
protected:
    T value_;
    std::function<T(T)> coerce_ {[](T val) { return val; }};
    std::function<void(T)> hook_ = [](T) { gRoot->remakeAll("ParamWrapper"); };

    friend QcrControl<T>;
    void guiSetsVal(T, bool userCall=false);
    std::function<void(T)> setGuiVal_ = [](T){};
};

//  ***********************************************************************************************
//  class ParamWrapper implementation

template<class T>
void ParamWrapper<T>::setVal(T val)
{
    T newval = coerce_(val);
    if (newval!=val)
        qDebug() << "Value " << val << " corrected into " << newval;
    value_ = newval;
    setGuiVal_(newval);
    // qDebug() << " -> " << val << " -> " << newval <<  " (non-user call)";
}

template<class T>
void ParamWrapper<T>::guiSetsVal(T val, bool userCall)
{
    if (coerce_(val) != val) {
        QByteArray tmp1 = strOp::to_s(val).toLatin1();
        QByteArray tmp2 = strOp::to_s(coerce_(val)).toLatin1();
        qFatal("GUI delivered value %s that ought to be coerced to %s",
               tmp1.constData(), tmp2.constData());
    }
    value_ = val;
    if (userCall) {
        // qDebug() << " -> " << val;
        hook_(val);
    } else {
        ;// qDebug() << " -> " << val << " (non-user call)";
    }
}

#endif // PARAM_WRAPPER_H
