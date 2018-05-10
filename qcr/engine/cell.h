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

#include <list>

//! Manages update dependences.
class Cell
{
public:
    Cell();
    typedef long int stamp_t;
protected:
    static stamp_t latestTimestamp__ { 0 }
    static stamp_t mintTimestamp() { return ++latestTimestamp__; }
private:
    stamp_t timestamp_ { 0 }
};

class FinalCell : public Cell;

#endif // CELL_H
