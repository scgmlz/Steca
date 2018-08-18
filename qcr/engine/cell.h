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

//! Holds a single value, and a hook that is executed when the Gui changes the value.
template<class T>
class QcrCell {
public:
    QcrCell(T value) : value_{value} {}
    template<class Q> QcrCell(Q value) = delete; // prevent automatic conversion Q->T
    QcrCell(const QcrCell&) = default;

    void setVal(const T);
    void setCoerce  (std::function<T   (const T)> f) { coerce_ = f; }
    void setHook    (std::function<void(const T)> f) { hook_ = f; }
    void setCallback(std::function<void(const T)> f) { callback_ = f; }

    T val() const { return value_; }

private:
    T value_;
    std::function<T   (const T)> coerce_   = [](const T v){ return v;};
    std::function<void(const T)> hook_     = [](const T)  {;};
    std::function<void(const T)> callback_ = [](const T)  {;};
};

//  ***********************************************************************************************
//  class QcrCell implementation

template<class T>
void QcrCell<T>::setVal(const T val)
{
    value_ = coerce_(val);
    callback_(value_);
    hook_(val);
}

#endif // CELL_H
