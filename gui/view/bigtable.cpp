// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/view/bigtable.cpp
//! @brief     Implements classes DataView
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "bigtable.h"
#include "core/def/debug.h"
#include "core/def/idiomatic_for.h"
#include "core/session.h"
#include "gui/mainwin.h"
#include "gui/state.h"
#include <QApplication>
#include <QClipboard>
#include <QHeaderView>
#include <QKeyEvent>

// ************************************************************************** //
//  class DataModel
// ************************************************************************** //

DataModel::DataModel()
    : TableModel("data#")
{
    headers_ = PeakInfo::dataTags(false);
    cmpFunctions_ = PeakInfo::dataCmps();
    ASSERT(cmpFunctions_.count() == headers_.count());
    numCols_ = headers_.count();
    colIndexMap_.resize(numCols_);
    for_i (numCols_)
        colIndexMap_[i] = i;
}

//! The first column contains row numbers. The remaining numCols columns contain data.

QVariant DataModel::data(const QModelIndex& index, int role) const
{
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
            const QVariant& var = rows_.at(indexRow).row.at(indexCol);
            if (isNumeric(var))
                return Qt::AlignRight;
        }

        return Qt::AlignLeft;

    default: break;
    }

    return QVariant();
}

QVariant DataModel::headerData(int section, Qt::Orientation, int role) const
{
    if (section < 0 || headers_.count() < section)
        return QVariant();

    if (Qt::DisplayRole == role)
        return 0 == section ? "#" : headers_.at(section - 1);

    return QVariant();
}

void DataModel::moveColumn(int from, int to)
{
    ASSERT(from < colIndexMap_.count() && to < colIndexMap_.count());
    qSwap(colIndexMap_[from], colIndexMap_[to]);
}

void DataModel::setSortColumn(int col)
{
    sortColumn_ = col < 0 ? col : colIndexMap_.at(col);
}

void DataModel::clear()
{
    beginResetModel();
    rows_.clear();
    endResetModel();
}

void DataModel::addRow(const row_t& row, bool sort)
{
    rows_.append(numRow(rows_.count() + 1, row));
    if (sort)
        sortData();
}

const row_t& DataModel::row(int index)
{
    return rows_.at(index).row;
}

void DataModel::sortData()
{
    auto _cmpRows = [this](int col, const row_t& r1, const row_t& r2) {
        col = colIndexMap_.at(col);
        return cmpFunctions_.at(col)(r1.at(col), r2.at(col));
    };

    // sort by sortColumn first, then left-to-right
    auto _cmp = [this, _cmpRows](const numRow& r1, const numRow& r2) {
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

DataView::DataView()
    : TableView {new DataModel}
{
    outHeaders_ = PeakInfo::dataTags(true);

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

    // inbound connections:
    connect(gSession, &Session::sigBigtableCols, this, &DataView::updateShownColumns);

    // internal connections:
    connect(
        header(), &QHeaderView::sectionMoved,
        [this](int /*logicalIndex*/, int oldVisualIndex, int newVisualIndex) {
            ASSERT(oldVisualIndex > 0 && newVisualIndex > 0);
            header()->setSortIndicatorShown(false);
            model()->moveColumn(oldVisualIndex-1, newVisualIndex-1);
            model()->sortData();
        });

    connect(header(), &QHeaderView::sectionClicked, [this](int logicalIndex) {
        QHeaderView* h = header();
        h->setSortIndicatorShown(true);
        h->setSortIndicator(logicalIndex, Qt::AscendingOrder);
        model()->setSortColumn(logicalIndex-1);
        model()->sortData();
    });
}

void DataView::refresh()
{
    model()->clear();
    for (const PeakInfo& r : gSession->peakInfos())
        model()->addRow(r.data(), false);
    model()->sortData();
}

void DataView::updateShownColumns()
{
    int nCol = model()->columnCount();
    for_i (nCol-1) {
        if (gGui->state->bigtableShowCol[i])
            showColumn(i + 1);
        else
            hideColumn(i + 1);
    }
}

//! To enable copying to external applications
void DataView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_C && event->modifiers() & Qt::ControlModifier) {
        QApplication::clipboard()->setText(exportSelection());
    } else {
        QTreeView::keyPressEvent(event);
    }
}

//! Encodes selected items as a string with separators '\t' and '\n', for use in keyPressEvent.
QString DataView::exportSelection() const
{
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
        ret.append(model()->data(current, Qt::DisplayRole).toString());
        previous_row = current.row();
    }
    return ret;
}

void DataView::toFile(QTextStream& stream, const QString& separator) const
{
    const QStringList& headers = outHeaders();
    for_i (headers.count())
        stream << headers.at(i) << separator;
    stream << '\n';

    for_i (model()->columnCount()) {
        const row_t& r = model()->row(i);
        for_i (r.count()) {
            const QVariant& var = r.at(i);
            if (isNumeric(var))
                stream << var.toDouble();
            else
                stream << var.toString();
            stream << separator;
        }
        stream << '\n';
    }
}
