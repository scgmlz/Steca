// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/output/data_table.cpp
//! @brief     Implements classes DataTable
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/output/data_table.h"
#include "core/def/idiomatic_for.h"
#include "gui/base/table_model.h"
#include "gui/output/dialog_panels.h"
#include <QHeaderView>


// ************************************************************************** //
//  local class TabularModel
// ************************************************************************** //

class TabularModel : public TableModel {
public:
    TabularModel(uint numCols_);

    int columnCount() const final { return to_i(numCols_) + 1; }
    int rowCount() const final { return to_i(rows_.count()); }

    QVariant data(const QModelIndex&, int) const;
    QVariant headerData(int, Qt::Orientation, int) const;

    void moveColumn(uint from, uint to);

    void setColumns(const QStringList& headers, cmp_vec const&);

    void setSortColumn(int);

    void clear();
    void addRow(row_t const&, bool sort = true);

    row_t const& row(uint);

    void sortData();

private:
    uint numCols_;
    int sortColumn_;

    QStringList headers_;
    uint_vec colIndexMap_;
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

TabularModel::TabularModel(uint numColumns_)
    : TableModel(), numCols_(numColumns_), sortColumn_(-1) {
    colIndexMap_.resize(numCols_);
    for_i (numCols_)
        colIndexMap_[i] = i;
}


// The first column contains row numbers. The rest numCols columns contain data.

QVariant TabularModel::data(const QModelIndex& index, int role) const {
    int indexRow = index.row(), indexCol = index.column();
    int numRows = rowCount(), numCols = columnCount();

    if (indexCol < 0 || indexRow < 0)
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        if (0 == indexCol)
            return rows_.at(to_u(indexRow)).n;

        if (--indexCol < numCols && indexRow < numRows) {
            QVariant var = rows_.at(to_u(indexRow)).row.at(to_u(indexCol));
            if (isNumeric(var) && qIsNaN(var.toDouble()))
                var = QVariant(); // hide nans
            return var;
        }

        break;

    case Qt::TextAlignmentRole:
        if (0 == indexCol)
            return Qt::AlignRight;

        if (--indexCol < numCols && indexRow < numRows) {
            QVariant const& var = rows_.at(to_u(indexRow)).row.at(to_u(indexCol));
            if (isNumeric(var))
                return Qt::AlignRight;
        }

        return Qt::AlignLeft;

    default: break;
    }

    return QVariant();
}

QVariant TabularModel::headerData(int section, Qt::Orientation, int role) const {
    if (section < 0 || headers_.count() < section)
        return QVariant();

    if (Qt::DisplayRole == role)
        return 0 == section ? "#" : headers_.at(to_u(section) - 1);

    return QVariant();
}

void TabularModel::moveColumn(uint from, uint to) {
    debug::ensure(from < colIndexMap_.count() && to < colIndexMap_.count());
    qSwap(colIndexMap_[from], colIndexMap_[to]);
}

void TabularModel::setColumns(const QStringList& headers, cmp_vec const& cmps) {
    debug::ensure(to_u(headers.count()) == numCols_ && cmps.count() == numCols_);
    headers_ = headers;
    cmpFunctions_ = cmps;
}

void TabularModel::setSortColumn(int col) {
    sortColumn_ = col < 0 ? col : colIndexMap_.at(col);
}

void TabularModel::clear() {
    beginResetModel();
    rows_.clear();
    endResetModel();
}

void TabularModel::addRow(row_t const& row, bool sort) {
    rows_.append(numRow(rows_.count() + 1, row));
    if (sort)
        sortData();
}

row_t const& TabularModel::row(uint index) {
    return rows_.at(index).row;
}

void TabularModel::sortData() {
    auto _cmpRows = [this](uint col, row_t const& r1, row_t const& r2) {
        col = colIndexMap_.at(col);
        return cmpFunctions_.at(col)(r1.at(col), r2.at(col));
    };

    // sort by sortColumn first, then left-to-right
    auto _cmp = [this, _cmpRows](numRow const& r1, numRow const& r2) {
        if (0 <= sortColumn_) {
            int c = _cmpRows(to_u(sortColumn_), r1.row, r2.row);
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
            if (to_i(col) != sortColumn_) {
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
//  class DataTable
// ************************************************************************** //

DataTable::DataTable(uint numDataColumns) : model_(nullptr) {
    model_.reset(new TabularModel(numDataColumns));
    setModel(model_.ptr());
    setHeader(new QHeaderView(Qt::Horizontal));

    QHeaderView* h = header();

    h->setSectionResizeMode(0, QHeaderView::Fixed);
    h->setSectionsMovable(true);
    h->setSectionsClickable(true);

    int w = QFontMetrics(h->font()).width("000000000");
    setColumnWidth(0, w);
}

void DataTable::setColumns(
    const QStringList& headers, const QStringList& outHeaders, cmp_vec const& cmps) {
    model_->setColumns(headers, cmps);
    debug::ensure(headers.count() == outHeaders.count());
    outHeaders_ = outHeaders;

    connect(
        header(), &QHeaderView::sectionMoved,
        [this](int /*logicalIndex*/, int oldVisualIndex, int newVisualIndex) {
            debug::ensure(oldVisualIndex > 0 && newVisualIndex > 0);
            header()->setSortIndicatorShown(false);
            model_->moveColumn(to_u(oldVisualIndex - 1), to_u(newVisualIndex - 1));
            model_->sortData();
        });

    connect(header(), &QHeaderView::sectionClicked, [this](int logicalIndex) {
        QHeaderView* h = header();
        h->setSortIndicatorShown(true);
        h->setSortIndicator(logicalIndex, Qt::AscendingOrder);
        model_->setSortColumn(logicalIndex - 1);
        model_->sortData();
    });
}

void DataTable::clear() {
    model_->clear();
}

void DataTable::addRow(row_t const& row, bool sort) {
    model_->addRow(row, sort);
}

void DataTable::sortData() {
    model_->sortData();
}

uint DataTable::rowCount() const {
    return to_u(model_->rowCount());
}

const row_t& DataTable::row(uint i) const {
    return model_->row(i);
}
