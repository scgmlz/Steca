//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/meta/meta_selection.h
//! @brief     Defines class MetaSelection
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef META_SELECTION_H
#define META_SELECTION_H

#include "core/typ/cached.h"
#include "qcr/engine/cell.h"
#include <vector>

//! Selection of meta parameters

class MetaSelection {
public:
    MetaSelection();
    MetaSelection(const MetaSelection&) = delete;
    MetaSelection(MetaSelection&&) = default;

    void set(int, bool);

    bool isSelected(int i) const { return vec.at(i).val(); }
    int numSelected() const { return list.get(this).size(); }
    int selectedOf(int i) const { return list.get(this).at(i); }

    std::vector<QcrCell<bool>> vec; //!< true if to be displayed
private:
    Kached<MetaSelection,std::vector<int>> list; //!< indices of metadata items selected for display
};

#endif // META_SELECTION_H
