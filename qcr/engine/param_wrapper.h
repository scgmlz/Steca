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
#include <QObject>
#include <functional>
#include <vector>

void remakeAll();

template<class T>
class QcrControl;

//! Holds a single data value, and functions to be run upon change
template<class T>
class ParamWrapper {
public:
    ParamWrapper() = delete;
    ParamWrapper(T value) : value_{value} {}
    T val() const { return value_; }
    void setCoerce(std::function<T(T)> coerce) { coerce_ = coerce; }
    void setPostHook(std::function<void(T)> postHook) { postHook_ = postHook; }
    void setVal(T);
    void reCoerce() { setVal(value_); }
private:
    T value_;
    std::function<void(T)> postHook_ = [](T) {};
    std::function<T(T)> coerce_ = [](T val) { return val; };

    friend QcrControl<T>;
    void guiSetsVal(T, bool userCall=false);
    std::function<void(T)> callGuiOnSet_ = [](T){};
};

//  ***********************************************************************************************
//  class ParamWrapper implementation

template<class T>
void ParamWrapper<T>::setVal(T val)
{
    T newval = coerce_(val);
    if (newval==value_) {
        qDebug() << " == " << val << " -> " << newval << " (as before)";
        return;
    }
    value_ = newval;
    callGuiOnSet_(newval);
    qDebug() << " -> " << val << " -> " << newval <<  " (non-user call)";
}

template<class T>
void ParamWrapper<T>::guiSetsVal(T val, bool userCall)
{
    T newval = coerce_(val);
    if (newval==value_) {
        qDebug() << " == " << val << " (as before)";
        return;
    }
    value_ = newval;
    if (userCall) {
        qDebug() << " -> " << val;
        postHook_(newval);
        remakeAll();
    } else {
        qDebug() << " -> " << val << " (non-user call)";
    }
}

#endif // PARAM_WRAPPER_H
