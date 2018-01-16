// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/base/tree_views.cpp
//! @brief     Implements classes TreeView, ListView
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "tree_views.h"
#include "core/def/idiomatic_for.h"
#include "gui/base/table_model.h"

// ************************************************************************** //
//  class TreeView
// ************************************************************************** //

TreeView::TreeView() {
    setAlternatingRowColors(true);
}

int TreeView::mWidth() const {
    QFont f = font();
    f.setBold(false);
    return QFontMetrics(f).width('m');
}

// ************************************************************************** //
//  class ListView
// ************************************************************************** //

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual" // TODO try without
void ListView::setModel(TableModel* model) {
    TreeView::setModel(model);
    hideColumn(0); // this should look like a list; 0th column is tree-like

    if (model) {
        connect(model, &QAbstractItemModel::modelReset, [this, model]() {
            for_i (model->columnCount())
                resizeColumnToContents(i);
        });
    }
}
#pragma GCC diagnostic pop

TableModel* ListView::model() const {
    return static_cast<TableModel*>(TreeView::model());
}

void ListView::updateSingleSelection() {
    int row = currentIndex().row();
    model()->signalReset();
    selectRow(row);
}

void ListView::selectRow(int row) {
    setCurrentIndex(model()->index(row, 0));
}
