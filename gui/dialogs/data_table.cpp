// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/data_table.cpp
//! @brief     Implements classes DataView
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/dialogs/data_table.h"
#include "core/def/idiomatic_for.h"
#include <QApplication>
#include <QClipboard>
#include <QHeaderView>
#include <QKeyEvent>

// ************************************************************************** //
//  class DataModel
// ************************************************************************** //

DataModel::DataModel(int numColumns_)
    : TableModel("data"), numCols_(numColumns_), sortColumn_(-1)
{
    colIndexMap_.resize(numCols_);
    for_i (numCols_)
        colIndexMap_[i] = i;
}


// The first column contains row numbers. The rest numCols columns contain data.

QVariant DataModel::data(const QModelIndex& index, int role) const {
    int indexRow = index.row(), indexCol = index.column();
    int numRows = rowCount(), numCols = columnCount();

    if (indexCol < 0 || indexRow < 0)
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        if (0 == indexCol)
            return rows_.at(indexRow).n;

        if (--indexCol < numCols && indexRow < numRows) {
            QVariant var = rows_.at(indexRow).row.at(indexCol);
            if (isNumeric(var) && qIsNaN(var.toDouble()))
                var = QVariant(); // hide nans
            return var;
        }

        break;

    case Qt::TextAlignmentRole:
        if (0 == indexCol)
            return Qt::AlignRight;

        if (--indexCol < numCols && indexRow < numRows) {
            QVariant const& var = rows_.at(indexRow).row.at(indexCol);
            if (isNumeric(var))
                return Qt::AlignRight;
        }

        return Qt::AlignLeft;

    default: break;
    }

    return QVariant();
}

QVariant DataModel::headerData(int section, Qt::Orientation, int role) const {
    if (section < 0 || headers_.count() < section)
        return QVariant();

    if (Qt::DisplayRole == role)
        return 0 == section ? "#" : headers_.at(section - 1);

    return QVariant();
}

void DataModel::moveColumn(int from, int to) {
    ASSERT(from < colIndexMap_.count() && to < colIndexMap_.count());
    qSwap(colIndexMap_[from], colIndexMap_[to]);
}

void DataModel::setColumns(const QStringList& headers, cmp_vec const& cmps) {
    ASSERT(headers.count() == numCols_ && cmps.count() == numCols_);
    headers_ = headers;
    cmpFunctions_ = cmps;
}

void DataModel::setSortColumn(int col) {
    sortColumn_ = col < 0 ? col : colIndexMap_.at(col);
}

void DataModel::clear() {
    beginResetModel();
    rows_.clear();
    endResetModel();
}

void DataModel::addRow(row_t const& row, bool sort) {
    rows_.append(numRow(rows_.count() + 1, row));
    if (sort)
        sortData();
}

row_t const& DataModel::row(int index) {
    return rows_.at(index).row;
}

void DataModel::sortData() {
    auto _cmpRows = [this](int col, row_t const& r1, row_t const& r2) {
        col = colIndexMap_.at(col);
        return cmpFunctions_.at(col)(r1.at(col), r2.at(col));
    };

    // sort by sortColumn first, then left-to-right
    auto _cmp = [this, _cmpRows](numRow const& r1, numRow const& r2) {
        if (0 <= sortColumn_) {
            int c = _cmpRows(sortColumn_, r1.row, r2.row);
            if (c < 0)
                return true;
            if (c > 0)
                return false;
        } else if (-1 == sortColumn_) {
            if (r1.n < r2.n)
                return true;
            if (r1.n > r2.n)
                return false;
        }

        for_int (col, numCols_) {
            if (col != sortColumn_) {
                int c = _cmpRows(col, r1.row, r2.row);
                if (c < 0)
                    return true;
                if (c > 0)
                    return false;
            }
        }

        return false;
    };

    beginResetModel();
    std::sort(rows_.begin(), rows_.end(), _cmp);
    endResetModel();
}

// ************************************************************************** //
//  class DataView
// ************************************************************************** //

DataView::DataView(int numDataColumns)
    : model_(new DataModel(numDataColumns)) {
    setModel(model_.get());
    setHeader(new QHeaderView(Qt::Horizontal));
    setAlternatingRowColors(true);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::ContiguousSelection);

    QHeaderView* h = header();

    h->setSectionResizeMode(0, QHeaderView::Fixed);
    h->setSectionsMovable(true);
    h->setSectionsClickable(true);

    int w = QFontMetrics(h->font()).width("000000000");
    setColumnWidth(0, w);
}

void DataView::setColumns(
    const QStringList& headers, const QStringList& outHeaders, cmp_vec const& cmps) {
    model_->setColumns(headers, cmps);
    ASSERT(headers.count() == outHeaders.count());
    outHeaders_ = outHeaders;

    connect(
        header(), &QHeaderView::sectionMoved,
        [this](int /*logicalIndex*/, int oldVisualIndex, int newVisualIndex) {
            ASSERT(oldVisualIndex > 0 && newVisualIndex > 0);
            header()->setSortIndicatorShown(false);
            model_->moveColumn(oldVisualIndex-1, newVisualIndex-1);
            model_->sortData();
        });

    connect(header(), &QHeaderView::sectionClicked, [this](int logicalIndex) {
        QHeaderView* h = header();
        h->setSortIndicatorShown(true);
        h->setSortIndicator(logicalIndex, Qt::AscendingOrder);
        model_->setSortColumn(logicalIndex-1);
        model_->sortData();
    });
}

void DataView::clear() {
    model_->clear();
}

void DataView::addRow(row_t const& row, bool sort) {
    model_->addRow(row, sort);
}

void DataView::sortData() {
    model_->sortData();
}

int DataView::rowCount() const {
    return model_->rowCount();
}

const row_t& DataView::row(int i) const {
    return model_->row(i);
}

void DataView::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_C && event->modifiers() & Qt::ControlModifier) {
        QApplication::clipboard()->setText(exportSelection());
    } else {
        QTreeView::keyPressEvent(event);
    }
}

QString DataView::exportSelection() {
    // TODO: improve https://stackoverflow.com/questions/1230222
    QString ret;
    QModelIndexList indexes = selectionModel()->selectedIndexes();
    int previous_row = -1;
    foreach(QModelIndex current, indexes) {
        if (previous_row==-1)
            ; // pass
        else if (current.row() != previous_row)
            ret.append('\n');
        else
            ret.append('\t');
        ret.append(model_->data(current, Qt::DisplayRole).toString());
        previous_row = current.row();
    }
    return ret;
}
