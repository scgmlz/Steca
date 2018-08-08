//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/cell.h
//! @brief     Defines and implements templated class QcrCell
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
#include "qcr/base/string_ops.h"
#include "qcr/engine/mixin.h"
#include <QObject>
#include <functional>
#include <vector>

template<class T>
class QcrControl;

//! Holds a single parameter.
template<class T>
class QcrCell {
public:
    QcrCell() = delete;
    QcrCell(T value) : value_{value} {}

    void setVal(T);
    void setHook(std::function<void(T)> hook) { hook_ = hook; }

    T val() const { return value_; }

private:
    T value_;
    std::function<void(T)> hook_ = [](T) { gRoot->remakeAll("QcrCell"); };

    friend QcrControl<T>;
    void guiSetsVal(T, bool userCall=false);
};

//  ***********************************************************************************************
//  class QcrCell implementation

template<class T>
void QcrCell<T>::setVal(T val)
{
    value_ = val;
}

template<class T>
void QcrCell<T>::guiSetsVal(T val, bool userCall)
{
    value_ = val;
    if (userCall) // to prevent circular calls; TODO simplify
        hook_(val);
}

#endif // CELL_H
