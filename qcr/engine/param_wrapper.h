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

//! Holds a single parameter.
template<class T>
class ParamWrapper {
public:
    ParamWrapper() = delete;
    ParamWrapper(T value) : value_{value}, coerce_{[this](T val) { return val; }} {}

    void setVal(T);
    T val() const { return value_; }

    void setPostHook(std::function<void(T)> postHook) { postHook_ = postHook; }
private:
    T value_;
    std::function<T(T)> coerce_;
    std::function<void(T)> postHook_ = [](T) {};

    friend QcrControl<T>;
    void guiSetsVal(T, bool userCall=false);
    std::function<void(T)> callGuiOnSet_ = [](T){};
};

//! Holds a single number.
template<class T>
class NumberWrapper : public ParamWrapper {
public:
    NumberWrapper(T value) : value_{value}, coerce_{[this](T val) { return coerceMinMax(val); }} {}

private:
    bool hasMin_ {false};
    bool hasMax_ {false};
    std::function<void(T)> minFct_ = [](T) { return T(); };
    std::function<void(T)> maxFct_ = [](T) { return T(); };
    T coerceMinMax(T val);
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

//  ***********************************************************************************************
//  class NumberWrapper implementation

template<class T>
T NumberWrapper<T>::coerceMinMax(T val)
{
    T ret = val;
    if (hasMin_)
        ret = std::max(val, minFct_());
    if (hasMax_)
        ret = std::min(val, maxFct_());
    return ret;
}

#endif // PARAM_WRAPPER_H
