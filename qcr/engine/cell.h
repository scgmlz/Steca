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

#include "qcr/base/debug.h"
#include <functional>
#include <memory>

//! Holds a single value_, and a hook_ that is executed when the value has changed.

//! Intended for use by a control widget that owns the cell.
//! To interact with that owning widget, there are "set" and "get" callbacks.
template<class T>
class QcrCell {
public:
    QcrCell(T value) : value_{value} {}
    template<class Q> QcrCell(Q value) = delete; // prevent automatic conversion Q->T
    QcrCell(const QcrCell&) = delete;
    QcrCell(QcrCell&&) = default;

    //! Sets value_ of cell, calls "set" callback, and runs hook_ if value has changed.
    void setVal(const T);
    //! Sets value_ of cell, calls "set" callback, but does not calls hook.
    void pureSetVal(const T);
    //! Sets a function that transforms an arbitrary input into an allowed value.
    void setCoerce  (std::function<T   (const T)> f) { coerce_ = f; }
    //! Sets a function that is called whenever function setValue changed the current value_.
    void setHook    (std::function<void(const T)> f) { hook_ = f; }
    //! Sets callback functions, intended to set and get the owning widget's value.
    void setCallbacks(std::function<T   (       )> _get, std::function<void(const T)> _set) {
        callbackGet_.reset(new std::function<T()>{_get});
        callbackSet_ = _set;
    }
    //! Resets callback functions to initial state.
    void releaseCallbacks() {
        callbackGet_.release();
        callbackSet_ = [](const T) {};
    }
    //! Returns the current value_.
    T val() const { return value_; }
    //! Returns true if and only if we are calling a "set" or "get" callback function.
    bool amCalling() const;
private:
    T value_;
    bool amCalling_ {false};
    std::function<T   (const T)> coerce_      = [](const T v){ return v;};
    std::function<void(const T)> hook_        = [](const T) {};
    std::unique_ptr<std::function<T()>> callbackGet_;
    std::function<void(const T)> callbackSet_ = [](const T) {};
};

//  ***********************************************************************************************
//  class QcrCell implementation

template<class T>
void QcrCell<T>::setVal(const T v)
{
    // qDebug() << QString("Cell::setVal new=%1, old=%2").arg(v).arg(value_);
    const T oldvalue = value_;
    pureSetVal(v);
    // qDebug() << QString("Cell::setVal after pure set val=%1").arg(v);
    if (value_!=oldvalue) {
        hook_(value_);
        // qDebug() << QString("Cell::setVal hook val=%1").arg(v);
    }
}

template<class T>
void QcrCell<T>::pureSetVal(const T v)
{
    value_ = coerce_(v);
    if (!callbackGet_)
        return; // no owning widget => nothing to do
    amCalling_ = true;
    callbackSet_(value_); // set value of owning widget
    value_ = (*callbackGet_)();
    amCalling_ = false;
}

//! Returns true if and only if we are calling a "set" or "get" callback function.

//! This helps the owning widget to find out whether its value has been changed
//! programatically by its cell, or through direct user action.
template<class T>
bool QcrCell<T>::amCalling() const
{
    return amCalling_;
}

#endif // CELL_H
