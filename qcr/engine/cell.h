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

#include <functional>
#include <set>
#include <vector>

//! Manages update dependences.
class Cell
{
public:
    Cell() {}
    typedef long int stamp_t;
    stamp_t update();
    void add_source(Cell*);
    void rm_source(Cell*);
    void connectAction(std::function<void()>&&);
protected:
    virtual void recompute() = 0;
    void actOnChange();
private:
    stamp_t timestamp_ { 0 };
    std::set<Cell*> sources_;
    std::vector<std::function<void()>> actionsOnChange_;
};

class FinalCell : public Cell {
private:
    static stamp_t latestTimestamp__;
    static stamp_t mintTimestamp() { return ++latestTimestamp__; }
    void recompute() final {};
};

//! Holds a single data value, and functions to be run upon change
template<class T>
class ParamCell : public Cell {
public:
    T getParam() const { return value_; }
    void setParam(T val) {
        if (val==value_)
            return;
        value_ = val;
        actOnChange();
    }
private:
    T value_;
    void recompute() final {};
};

#endif // CELL_H
