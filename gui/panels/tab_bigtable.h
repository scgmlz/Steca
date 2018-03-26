// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/tab_bigtable.h
//! @brief     Defines class BigtableTab, and its dependences
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TAB_TABLE_H
#define TAB_TABLE_H

#include "gui/panels/tab_mainframe.h"

//! Tabular display of fit results and metadata, with associated controls.

class BigtableTab : public TabMainframe {
public:
    BigtableTab();
    const class DataView& dataView() const { return *dataView_; }
    void render() final;
private:
    class DataView* dataView_;
};

#endif // TAB_TABLE_H
