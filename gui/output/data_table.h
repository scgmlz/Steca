// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/output/data_table.h
//! @brief     Defines classes DataTable
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef DATA_TABLE_H
#define DATA_TABLE_H

#include "core/typ/variant.h"
#include "core/def/special_pointers.h"
#include <QTreeView>

//! A data table view, for use in the 'Points' tab of an output Frame.

class DataTable : public QTreeView {
public:
    DataTable(int numDataColumns);

    void clear();
    void setColumns(const QStringList& headers, const QStringList& outHeaders, cmp_vec const&);
    void addRow(row_t const&, bool sort);
    void sortData();

    QStringList outHeaders() const { return outHeaders_; }
    int rowCount() const;
    row_t const& row(int) const;

private:
    scoped<class TabularModel*> model_;
    QStringList outHeaders_;
};

#endif // DATA_TABLE_H
