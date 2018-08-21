//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/cell.h
//! @brief     Defines and implements class template QcrCell
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#ifndef CELL_H
#define CELL_H

//#include "qcr/base/debug.h"
#include <functional>
#include <memory>

//! Holds a single value, and a hook that is executed when the value has changed.
template<class T>
class QcrCell {
public:
    QcrCell(T value) : value_{value} {}
    template<class Q> QcrCell(Q value) = delete; // prevent automatic conversion Q->T
    QcrCell(const QcrCell&) = delete;
    QcrCell(QcrCell&&) = default;

    void setVal(const T);
    void pureSetVal(const T);
    void setCoerce  (std::function<T   (const T)> f) { coerce_ = f; }
    void setHook    (std::function<void(const T)> f) { hook_ = f; }
    void setCallbacks(
        std::function<T   (       )> _get,
        std::function<void(const T)> _set)
        {
            callbackGet_.reset(new std::function<T()>{_get});
            callbackSet_ = _set;
        }

    T val() const { return value_; }
    bool amCalling {false};
private:
    T value_;
    std::function<T   (const T)> coerce_      = [](const T v){ return v;};
    std::function<void(const T)> hook_        = [](const T)  {;};
    std::unique_ptr<std::function<T()>> callbackGet_;
    std::function<void(const T)> callbackSet_ = [](const T)  {;};
};

//  ***********************************************************************************************
//  class QcrCell implementation

//! Sets value of cell, sets value of owning widget, and runs hook if value has changed.
template<class T>
void QcrCell<T>::setVal(const T v)
{
    const T oldvalue = value_;
    pureSetVal(v);
    if (value_!=oldvalue)
        hook_(value_);
}

//! Sets value of cell, calls callback to set value of controlling widget, but does not calls hook.
template<class T>
void QcrCell<T>::pureSetVal(const T v)
{
    value_ = coerce_(v);
    if (!callbackGet_)
        return; // no owning widget => nothing to do
    amCalling = true;
    callbackSet_(value_); // set value of owning widget
    value_ = (*callbackGet_)();
    amCalling = false;
}

#endif // CELL_H
