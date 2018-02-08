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
#include "gui/console.h"

// ************************************************************************** //
//  class TableModel
// ************************************************************************** //

// ************************************************************************** //
//  class TableView
// ************************************************************************** //

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual" // TODO try without

TableView::TableView(const QString& name, TableModel* model)
    : name_(name), model_(model)
{
    // set model
    ASSERT(model);
    QTreeView::setModel(model);
    hideColumn(0); // this should look like a list; 0th column is tree-like
    connect(model, &QAbstractItemModel::modelReset, [this, model]() {
            for_i (model->columnCount())
                resizeColumnToContents(i);
        });
    // other settings
    setSelectionMode(QAbstractItemView::NoSelection);
    setAlternatingRowColors(true);
}

#pragma GCC diagnostic pop

int TableView::mWidth() const {
    QFont f = font();
    f.setBold(false);
    return QFontMetrics(f).width('m');
}

//! Sets highlight when mouse or key action made an item current.

//! Called by a child class's currentChanged, which overrides QAbstractItemView::currentChanged.
//! We cannot place currentChanged in this pure virtual class, because then it would be
//! called by the constructor, which would result in undefined behavior.

void TableView::gotoCurrent(QModelIndex const& current) {
    if (current.row()==data_highlighted())
        return; // the following would prevent execution of "onClicked"
    data_setHighlight(current.row());
    updateScroll();
}

//! Highlights one cluster. Called either from GUI > currentChanged, or through Console command.
void TableView::highlight(bool primaryCall, int row) {
    if (row==data_highlighted())
        return; // the following would prevent execution of "onClicked"
    gConsole->log2(primaryCall, name_+".highlight="+QString::number(row));
    data_setHighlight(row);
    updateScroll();
}

void TableView::updateScroll() {
    int row = data_highlighted();
    if (row>=0)
        scrollTo(model()->index(row,0));
}
