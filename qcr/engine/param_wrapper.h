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
    void setHook(std::function<void(T)> hook) { hook_ = hook; }

    T val() const { return value_; }

private:
    T value_;
    std::function<void(T)> hook_ = [](T) { gRoot->remakeAll("ParamWrapper"); };

    friend QcrControl<T>;
    void guiSetsVal(T, bool userCall=false);
};

//  ***********************************************************************************************
//  class ParamWrapper implementation

template<class T>
void ParamWrapper<T>::setVal(T val)
{
    value_ = val;
}

template<class T>
void ParamWrapper<T>::guiSetsVal(T val, bool userCall)
{
    value_ = val;
    if (userCall) // to prevent circular calls; TODO simplify
        hook_(val);
}

#endif // PARAM_WRAPPER_H
