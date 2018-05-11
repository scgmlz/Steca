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

#include <set>

//! Manages update dependences.
class Cell
{
public:
    Cell() {}
    typedef long int stamp_t;
    stamp_t update();
    void add_source(Cell*);
    void rm_source(Cell*);
protected:
    virtual void recompute() = 0;
private:
    stamp_t timestamp_ { 0 };
    std::set<Cell*> sources_;
};

class FinalCell : public Cell {
private:
    static stamp_t latestTimestamp__;
    static stamp_t mintTimestamp() { return ++latestTimestamp__; }
    void recompute() final {};
};

/*
template<class T>
class ParamCell : public Cell {
public:
    T getParam() { return value_; }
    void setParam() { }
    void addClient(
private:
    T value_;
    void recompute() final {};
};
*/

#endif // CELL_H
