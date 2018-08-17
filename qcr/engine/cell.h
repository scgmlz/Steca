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

    void setVal(T);
    void setHook(std::function<void(T&)> f) { hook_ = f; }
    void setCallback(std::function<void(const T)> f) { callback_ = f; }

    T val() const { return value_; }
    void guiSetsVal(T);

private:
    T value_;
    std::function<void(T&)> hook_ = [](T&){;};
    std::function<void(const T)> callback_ = [](const T){;};
};

//  ***********************************************************************************************
//  class QcrCell implementation

template<class T>
void QcrCell<T>::setVal(T val)
{
    value_ = val;
    callback_(val);
}

template<class T>
void QcrCell<T>::guiSetsVal(T val)
{
    value_ = val;
    hook_(val);
    if (val!=value_) // hook_ may change val; this mechanism is used in RangeControl
        setVal(val);
}

#endif // CELL_H
