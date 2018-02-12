// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/output/data_table.h
//! @brief     Defines classes DataView
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
#include "gui/base/model_view.h"
#include <memory>
#include <QTreeView>

//! Model for the DataView view.

class DataModel : public TableModel {
public:
    DataModel(int numCols_);

    void clear();
    void moveColumn(int from, int to);
    void setColumns(const QStringList& headers, cmp_vec const&);
    void setSortColumn(int);
    void addRow(row_t const&, bool sort = true);
    void sortData();
    row_t const& row(int);

    int columnCount() const final { return numCols_ + 1; }
    int rowCount() const final { return rows_.count(); }
    int highlighted() const final { return 0; }// gSession->dataset().highlight().clusterIndex(); }
    void setHighlight(int i) final { ; } //gSession->dataset().highlight().setCluster(i); }

    QVariant data(const QModelIndex&, int) const;
    QVariant headerData(int, Qt::Orientation, int) const;

private:
    int numCols_;
    int sortColumn_;

    QStringList headers_;
    vec<int> colIndexMap_;
    cmp_vec cmpFunctions_;

    struct numRow {
        typedef numRow const& rc;
        numRow() : n(0), row() {}
        numRow(int n_, row_t const& row_) : n(n_), row(row_) {}
        int n;
        row_t row;
    };

    vec<numRow> rows_;
};

//! A data table view, for use in the 'Points' tab of an output Frame.

class DataView : public QTreeView {
public:
    DataView(int numDataColumns);

    void clear();
    void setColumns(const QStringList& headers, const QStringList& outHeaders, cmp_vec const&);
    void addRow(row_t const&, bool sort);
    void sortData();

    QStringList outHeaders() const { return outHeaders_; }
    int rowCount() const;
    row_t const& row(int) const;

private:
    std::unique_ptr<DataModel> model_;
    QStringList outHeaders_;
};

#endif // DATA_TABLE_H
