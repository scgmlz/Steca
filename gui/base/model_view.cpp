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

TableModel::TableModel(const QString& name)
    : name_(name)
{
    gConsole->learn(name_+".highlight", [this](const QString& val)->void {
            highlight(false, val.toInt()); });
}

void TableModel::refreshModel() {
    emit dataChanged(createIndex(0,0),createIndex(rowCount(),columnCount()-1));
}

//! Redraws the entire table, and sets currentIndex to (0,0) [?] which may be unwanted

//! Prefer this over dataChanged if and only rowCount may have shrinked,
//! and be aware that it resets the currentIndex so that arrow keys will start from row 0.

void TableModel::resetModel() {
    beginResetModel();
    endResetModel();
}

void TableModel::onClicked(const QModelIndex& cell) {
    int row = cell.row();
    int col = cell.column();
    if (row < 0 || row >= rowCount())
        return;
    setHighlight(row);
    gConsole->log(name() + ".highlight=" + QString::number(row));
}

// ************************************************************************** //
//  class CheckTableModel
// ************************************************************************** //

CheckTableModel::CheckTableModel(const QString& _name) : TableModel(_name) {
    gConsole->learn(name()+".activate", [this](const QString& val)->void {
            activateAndLog(false, val.toInt(), true); });
    gConsole->learn(name()+".deactivate", [this](const QString& val)->void {
            activateAndLog(false, val.toInt(), false); });
}

//! Refreshes the check box column.
void CheckTableModel::onActivated() {
    emit dataChanged(createIndex(0,1),createIndex(rowCount()-1,1));
}

void CheckTableModel::onClicked(const QModelIndex& cell) {
    TableModel::onClicked(cell);
    int row = cell.row();
    int col = cell.column();
    if (col==1)
        activateAndLog(true, row, !activated(row));
}

void CheckTableModel::activateAndLog(bool primaryCall, int row, bool on) {
    setActivated(row, on);
    gConsole->log2(primaryCall,
                   name() + ( on ? ".activate=" : ".deactivate=") + QString::number(row));
}



// ************************************************************************** //
//  class TableView
// ************************************************************************** //

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual" // TODO try without

TableView::TableView(TableModel* model)
    : name_(model->name()), model_(model)
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
    setHeaderHidden(true);
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
    if (current.row()==model_->highlighted())
        return; // the following would prevent execution of "onClicked"
    model_->setHighlight(current.row());
    updateScroll();
}

//! Highlights one cluster. Called either from GUI > currentChanged, or through Console command.
void TableView::highlight(bool primaryCall, int row) {
    if (row==model_->highlighted())
        return; // the following would prevent execution of "onClicked"
    gConsole->log2(primaryCall, name_+".highlight="+QString::number(row));
    model_->setHighlight(row);
    updateScroll();
}

void TableView::updateScroll() {
    int row = model_->highlighted();
    if (row>=0)
        scrollTo(model_->index(row,0));
}

void TableView::onHighlight() {
    model_->refreshModel();
    updateScroll();
}

void TableView::onData() {
    model_->resetModel();
    updateScroll();
}

// ************************************************************************** //
//  class CheckTableView
// ************************************************************************** //

void CheckTableView::onActivated() {
    model()->onActivated();
    updateScroll();
}
