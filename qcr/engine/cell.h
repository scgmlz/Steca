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
#include <vector>
#include <algorithm>

//! Holds a single value, and a hook that is executed when the value has changed.
template<class T>
class QcrCell {
public:
    QcrCell(T value) : value_{value} {}
    template<class Q> QcrCell(Q value) = delete; // prevent automatic conversion Q->T
    QcrCell(const QcrCell&) = delete;
    QcrCell(QcrCell&&) = default;

    void setVal(const T);
    void pureSetVal(const T v) { setVal(v); }
    void setCoerce  (std::function<T   (const T)> f) { coerce_ = f; }
    T val() const { return value_; }

    bool amCalling {false}; // make private?

    void addCallback(std::function<void(const T&)> callback) { callbacks_.push_back(callback); }

    bool removeCallback(const std::function<void(const T&)> &callback);

private:
    T value_;
    std::function<T   (const T)> coerce_      = [](const T v){ return v;};
    std::vector<std::function<void(const T&)>> callbacks_;

    void valueChanged(const T& v) {
        for (const auto &callback : callbacks_)
            callback(v);
    }
};

//  ***********************************************************************************************
//  class QcrCell implementation

template<class T>
bool QcrCell<T>::removeCallback(const std::function<void(const T&)> &callback)
{
    const auto it = std::find(callbacks_.begin(), callbacks_.end(), callback);
    if (it != callbacks_.end())
        callbacks_.erase(it);
    return it != callbacks_.end();
}

//! Sets value of cell, sets value of owning widget, and runs hook if value has changed.
template<class T>
void QcrCell<T>::setVal(const T v)
{
    const T oldvalue = value_;
    value_ = coerce_(v);
    if (value_!=oldvalue) {
        ASSERT(amCalling == false);
        amCalling = true;
        valueChanged(value_); // set value of owning widget
        amCalling = false;
    }
}

#endif // CELL_H
