//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/cell.h
//! @brief     Defines class Cell
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
#include <QObject>
#include <functional>
#include <set>
#include <vector>

void remakeAll();

//! Manages update dependences.
class Cell {
public:
    virtual ~Cell() = default; // needed as long as some Cells are explicitly deleted.
    void connectAction(std::function<void()>&&);
protected:
    void actOnChange();
private:
    std::vector<std::function<void()>> actionsOnChange_;
};

//! Holds a single data value, and functions to be run upon change
template<class T>
class ParamWrapper : public Cell {
public:
    ParamWrapper() = delete;
    ParamWrapper(T value) : value_(value) {}
    T val() const { return value_; }
    void setCoerce(std::function<T(T)> coerce) { coerce_ = coerce; }
    void setPostHook(std::function<void(T)> postHook) { postHook_ = postHook; }
    void setVal(T, bool userCall=false);
    void reCoerce() { setVal(value_); }
private:
    T value_;
    std::function<void(T)> postHook_ = [](T) {};
    std::function<T(T)> coerce_ = [](T val) { return val; };
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

#endif // CELL_H
