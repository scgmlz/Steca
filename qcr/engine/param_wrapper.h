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
    void connectAction(std::function<void()>&&);
    void setCoerce(std::function<T(T)> coerce) { coerce_ = coerce; }
    void setPostHook(std::function<void(T)> postHook) { postHook_ = postHook; }
    void setVal(T, bool userCall=false);
    void reCoerce() { setVal(value_); }
private:
    T value_;
    QcrControl<T>* widget_ {nullptr};
    std::function<void(T)> postHook_ = [](T) {};
    std::function<T(T)> coerce_ = [](T val) { return val; };
    void actOnChange();
    std::vector<std::function<void()>> actionsOnChange_;
};

//  ***********************************************************************************************
//  class ParamWrapper implementation

template<class T>
void ParamWrapper<T>::setVal(T val, bool userCall)
{
    T newval = coerce_(val);
    if (newval==value_) {
        qDebug() << " == " << val << " (as before)";
        return;
    }
    value_ = newval;
    actOnChange();
    if (userCall) {
        qDebug() << " -> " << val;
        postHook_(newval);
        remakeAll();
    } else {
        qDebug() << " -> " << val << " (non-user call)";
    }
}

//! Appends given function to actionsOnChange_.
template<class T>
void ParamWrapper<T>::connectAction(std::function<void()>&& f)
{
    actionsOnChange_.push_back(std::move(f));
}

template<class T>
void ParamWrapper<T>::actOnChange()
{
    for (const std::function<void()>& f: actionsOnChange_)
        f();
}

#endif // PARAM_WRAPPER_H
