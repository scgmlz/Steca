//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/raw/meta_selection.h
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

#include <vector>

//! Selection of meta parameters

class MetaSelection {
public:
    MetaSelection();
    void set(int, bool);
    bool isSelected(int i) const { return metaSelection_[i]; }
    int count() const { return metaInfoNums_.size(); }
    int at(int i) const { return metaInfoNums_.at(i); }
private:
    std::vector<bool> metaSelection_; //!< true if to be displayed
    std::vector<int> metaInfoNums_; //!< indices of metadata items selected for display
};

#endif // META_SELECTION_H
