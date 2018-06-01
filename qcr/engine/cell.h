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
    Cell() = delete;
    Cell(const QString& name) : name_(name) {}
    virtual ~Cell() = default; // needed as long as some Cells are explicitly deleted.
    typedef long unsigned int stamp_t;
    stamp_t update();
    void connectAction(std::function<void()>&&);
    const QString& name() const { return name_; }
protected:
    virtual void recompute() { qDebug() << "Dummy recompute of " << name(); }
    void actOnChange();
    stamp_t timestamp_ { 0 };
private:
    const QString name_;
    std::vector<std::function<void()>> actionsOnChange_;
};

class ValueCell : public Cell {
public:
    ValueCell(const QString& name) : Cell(name) {}
protected:
    static stamp_t latestTimestamp__;
    void timeStep() { timestamp_ = mintTimestamp(); }
private:
    static stamp_t mintTimestamp() { return ++latestTimestamp__; }
};

//! Holds a single data value, and functions to be run upon change
template<class T>
class SingleValueCell : public ValueCell {
public:
    SingleValueCell() = delete;
    SingleValueCell(const QString& name, T value) : ValueCell(name), value_(value) {}
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
//  class SingleValueCell implementation

template<class T>
void SingleValueCell<T>::setVal(T val, bool userCall)
{
    T newval = coerce_(val);
    if (newval==value_) {
        qDebug() << name() << " -> " << val << " (as before)";
        return;
    }
    value_ = newval;
    actOnChange();
    if (userCall) {
        timeStep();
        qDebug() << name() << " -> " << val << ", t=" << timestamp_;
        postHook_(newval);
        remakeAll();
    } else {
        qDebug() << name() << " -> " << val << " (non-user call)";
    }
}

#endif // CELL_H
