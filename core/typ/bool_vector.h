//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/bool_vector.h
//! @brief     Defines class BoolVector
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef BOOL_VECTOR_H
#define BOOL_VECTOR_H

#include "core/typ/cached.h"
#include "qcr/engine/cell.h"
#include <vector>

//! Selection of meta parameters

class BoolVector {
public:
    BoolVector();
    BoolVector(const BoolVector&) = delete;
    BoolVector(BoolVector&&) = default;

    void set(int, bool);

    bool isSelected(int i) const { return vec.at(i).val(); }
    int numSelected() const { return list.get(this).size(); }
    int selectedOf(int i) const { return list.get(this).at(i); }

    std::vector<QcrCell<bool>> vec; //!< true if to be displayed
private:
    Kached<BoolVector,std::vector<int>> list; //!< indices of metadata items selected for display
};

#endif // BOOL_VECTOR_H
