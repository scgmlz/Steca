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

//! Holds a single value, and a hook that is executed when the value has changed.
template<class T>
class QcrCell {
public:
    QcrCell(T value) : value_{value} {}
    template<class Q> QcrCell(Q value) = delete; // prevent automatic conversion Q->T
    QcrCell(const QcrCell&) = default;

    void setVal(const T);
    void pureSetVal(const T);
    void setCoerce  (std::function<T   (const T)> f) { coerce_ = f; }
    void setHook    (std::function<void(const T)> f) { hook_ = f; }
    void setCallback(std::function<void(const T)> f) { callback_ = f; }

    std::function<T   (const T)> coerce_   = [](const T v){ return v;};

    T val() const { return value_; }
private:
    T value_;
    std::function<void(const T)> hook_     = [](const T)  {;};
    std::function<void(const T)> callback_ = [](const T)  {;};
};

//  ***********************************************************************************************
//  class QcrCell implementation

//! Sets value of cell, sets value of owning widget, and runs hook if value has changed.
template<class T>
void QcrCell<T>::setVal(const T v)
{
    const T oldvalue = value_;
    value_ = coerce_(v);
    qDebug()<<"Cell setVal callback beg"<<oldvalue<<"->"<<v<<"-coerce->"<<value_;
    callback_(value_); // set value of owning widget
    qDebug()<<"Cell setVal callback end"<<oldvalue<<"->"<<v<<"-coerce->"<<value_;
    if (value_==v /* ultimately ensured by callback loop */ && value_!=oldvalue) {
        qDebug()<<"Cell setVal hook beg";
        hook_(value_);
        qDebug()<<"Cell setVal hook end";
    } else
        qDebug()<<"Cell setVal no hook"<<(value_==v)<<"&&"<<(value_!=oldvalue);
}

//! Sets value of cell, calls callback to set value of controlling widget, but does not calls hook.
template<class T>
void QcrCell<T>::pureSetVal(const T v)
{
    value_ = coerce_(v);
    qDebug()<<"Cell pureSetVal callback beg"<<v<<"->"<<value_;
    callback_(value_);
    qDebug()<<"Cell pureSetVal callback end";
}

#endif // CELL_H
