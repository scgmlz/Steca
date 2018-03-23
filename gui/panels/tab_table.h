// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/tab_table.h
//! @brief     Defines class TableWidget, and its dependences
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TAB_TABLE_H
#define TAB_TABLE_H

#include "gui/mainwin.h"
#include "gui/base/controls.h"
#include "gui/panels/data_table.h"
#include "core/algo/calc_polefigure.h" // includes peak_info.h

//! Tabular display of fit results and metadata, with associated controls.

class TableWidget : public QWidget {
public:
    TableWidget();
    const class DataView& dataView() const { return *dataView_; }
private:
    void render();
    class DataView* dataView_;
};

#endif // TAB_TABLE_H
