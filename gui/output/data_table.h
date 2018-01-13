// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/output/data_table.h
//! @brief     Defines classes DataTable, used in frame.cpp.
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef DATA_TABLE_H
#define DATA_TABLE_H

#include "core/typ/str.h"
#include "core/typ/variant.h"
#include "core/def/special_pointers.h"
#include "gui/base/tree_views.h"
#include "gui/base/new_q.h"

//! A data table, for use in the 'Points' tab of an output dialog.

class DataTable : public TreeView {
public:
    DataTable(uint numDataColumns);
    void setColumns(const QStringList& headers, const QStringList& outHeaders, cmp_vec const&);
    const QStringList outHeaders() { return outHeaders_; }
    void clear();
    void addRow(row_t const&, bool sort);
    void sortData();
    uint rowCount() const;
    row_t const& row(uint) const;
    scoped<class TabularModel*> model_;
    QStringList outHeaders_;
};

#endif // DATA_TABLE_H
