// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/base/model_view.cpp
//! @brief     Implements class TableView
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "model_view.h"
#include "core/def/idiomatic_for.h"

// ************************************************************************** //
//  class TableModel
// ************************************************************************** //

// ************************************************************************** //
//  class TableView
// ************************************************************************** //

TableView::TableView() {
    setAlternatingRowColors(true);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual" // TODO try without
void TableView::setModel(TableModel* model) {
    QTreeView::setModel(model);
    hideColumn(0); // this should look like a list; 0th column is tree-like
    if (model) {
        connect(model, &QAbstractItemModel::modelReset, [this, model]() {
            for_i (model->columnCount())
                resizeColumnToContents(i);
        });
    }
}
#pragma GCC diagnostic pop

TableModel* TableView::model() const {
    return static_cast<TableModel*>(QTreeView::model());
}

int TableView::mWidth() const {
    QFont f = font();
    f.setBold(false);
    return QFontMetrics(f).width('m');
}
