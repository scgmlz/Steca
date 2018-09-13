//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/view/bigtable.h
//! @brief     Defines classes BigtableView
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef BIGTABLE_H
#define BIGTABLE_H

#include "core/misc/variant.h"
#include "qcr/widgets/tables.h"

//! Model for the BigtableView view.

class BigtableModel : public TableModel {
public:
    BigtableModel();
    BigtableModel(const BigtableModel&) = delete;

    void refresh();
    void onColumnMove(int from, int to);
    void setSortColumn(int);
    void sortData();
    void onHighlight(int i) final { ; }        // unused

    QVariant data(const QModelIndex&, int) const;
    QVariant headerData(int, Qt::Orientation, int) const;
    int columnCount() const final { return numCols_ + 1; }
    int rowCount() const final { return rows_.size(); }
    const std::vector<QVariant>& row(int) const;
    QStringList getHeaders() const;
    std::vector<std::vector<const QVariant*>> getData() const;
    int highlighted() const final { return 0; } // unused

private:
    QStringList headers_;
    std::vector<int> colIndexMap_;
    std::vector<VariantComparator*> comparators_;

    // TODO: Treat the row number column as any other column, so that it can also be exported.
    // Then replace XRow by std::vector<QVariant>.
    struct XRow {
        XRow() {} // see https://bugreports.qt.io/browse/QTBUG-67357
        XRow(int n_, const std::vector<QVariant>& row_) : n(n_), row(row_) {}
        int n; // number of row = contents of first columns
        std::vector<QVariant> row;
    };

    int numCols_;
    int sortColumn_ {-1};
    std::vector<XRow> rows_;
};

//! A data table view, for use in the 'Points' tab of an output Frame.

class BigtableView : public TableView {
public:
    BigtableView();
    BigtableView(const BigtableView&) = delete;

    void refresh();

private:
    void updateShownColumns();
    void keyPressEvent(QKeyEvent *event);

    BigtableModel* model() const { return dynamic_cast<BigtableModel*>(model_); }
    QString exportSelection() const;
};

#endif // BIGTABLE_H
