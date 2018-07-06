//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/widgets/tables.cpp
//! @brief     Implements classes TableModel, CheckTableModel and TableView, CheckTableView
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#include "tables.h"
#include "qcr/engine/param_wrapper.h" // gRoot->remakeAll
#include "qcr/engine/console.h"
#include "qcr/base/qcrexception.h"
#include "qcr/base/debug.h"
#include "qcr/base/string_ops.h"

//  ***********************************************************************************************
//! @class TableModel

TableModel::TableModel(const QString& name)
    : name_(name)
{}

void TableModel::refreshModel()
{
    if (rowCountCached_==-1 || rowCount()<rowCountCached_) {
        // Redraws the entire table, and sets currentIndex to (0,0) [?].
        // Resets the currentIndex so that arrow keys will start from row 0.
        beginResetModel();
        endResetModel();
    } else {
        emit dataChanged(createIndex(0,0),createIndex(rowCount(),columnCount()-1));
    }
    rowCountCached_ = rowCount();
}

void TableModel::onClicked(const QModelIndex& cell)
{
    setHighlightedCell(cell);
}

void TableModel::setHighlightedCell(const QModelIndex& cell)
{
    int row = cell.row();
    if (row < 0 || row >= rowCount())
        return;
    setHighlight(row);
    gConsole->log(name() + " highlight " + QString::number(row));
}


//  ***********************************************************************************************
//! @class CheckTableModel

CheckTableModel::CheckTableModel(const QString& _name) : TableModel(_name)
{}

//! Refreshes the check box column. TODO currently unused
void CheckTableModel::onActivated()
{
    emit dataChanged(createIndex(0,1),createIndex(rowCount()-1,1));
}

void CheckTableModel::onClicked(const QModelIndex& cell)
{
    TableModel::setHighlightedCell(cell);
    int row = cell.row();
    int col = cell.column();
    if (col==1)
        activateAndLog(true, row, !activated(row));
    gRoot->remakeAll("CheckTableModel::onClicked");
}

void CheckTableModel::activateAndLog(bool primaryCall, int row, bool on)
{
    setActivated(row, on);
    gConsole->log2(primaryCall,
                   name() + ( on ? " activate " : " deactivate ") + QString::number(row));
}


//  ***********************************************************************************************
//! @class TableView

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual" // TODO try without

TableView::TableView(TableModel* model)
    : QcrSettable {*this, model->name()}
    , model_(model)
{
    // set model
    QTreeView::setModel(model);
    hideColumn(0); // this should look like a list; 0th column is tree-like
    connect(model, &QAbstractItemModel::modelReset, [this, model]() {
            for (int i=0; i<model->columnCount(); ++i)
                resizeColumnToContents(i);
        });
    connect(this, &TableView::clicked, model_, &TableModel::onClicked);

    // other settings
    setHeaderHidden(true);
    setSelectionMode(QAbstractItemView::NoSelection);
    setAlternatingRowColors(true);

    setRemake([this]() {onData();});
}

void TableView::executeConsoleCommand(const QString& arg)
{
    QString cmd, cmdarg;
    strOp::splitOnce(arg, cmd, cmdarg);
    if (cmd!="highlight")
        throw QcrException("Unexpected command '"+cmd+"' in TableModel "+name());
    if (cmdarg=="")
        throw QcrException("Missing argument to command 'highlight'");
    model_->setHighlight(strOp::to_i(cmdarg));
}

#pragma GCC diagnostic pop

int TableView::mWidth() const
{
    QFont f = font();
    f.setBold(false);
    return QFontMetrics(f).width('m');
}

//! Sets highlight when mouse or key action made an item current.

//! Called by a child class's currentChanged, which overrides QAbstractItemView::currentChanged.
//! We cannot place currentChanged in this pure virtual class, because then it would be
//! called by the constructor, which would result in undefined behavior.

void TableView::gotoCurrent(const QModelIndex& current)
{
    if (current.row()==model_->highlighted())
        return; // the following would prevent execution of "onClicked"
    model_->setHighlight(current.row());
    updateScroll();
}

//! Highlights one cluster. Called either from GUI > currentChanged, or through Console command.
void TableView::highlight(bool primaryCall, int row)
{
    if (row==model_->highlighted())
        return; // the following would prevent execution of "onClicked"
    gConsole->log2(primaryCall, name_+".highlight="+QString::number(row));
    model_->setHighlight(row);
    updateScroll();
}

void TableView::updateScroll()
{
    int row = model_->highlighted();
    if (row>=0)
        scrollTo(model_->index(row,0));
}

void TableView::onData()
{
    model_->refreshModel();
    updateScroll();
}


//  ***********************************************************************************************
//! @class CheckTableView

void CheckTableView::executeConsoleCommand(const QString& arg)
{
    QStringList args = arg.split(' ');
    if        (args[0]=="activate") {
        if (args.size()<2)
            throw QcrException("Missing argument to command 'activate'");
        model()->activateAndLog(false, strOp::to_i(args[1]), true);
    } else if (args[0]=="deactivate") {
        if (args.size()<2)
            throw QcrException("Missing argument to command 'deactivate'");
        model()->activateAndLog(false, strOp::to_i(args[1]), false);
    } else
        TableView::executeConsoleCommand(arg);
}
