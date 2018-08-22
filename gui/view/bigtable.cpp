//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/view/bigtable.cpp
//! @brief     Implements classes BigtableView
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/view/bigtable.h"
#include "core/session.h"
#include "gui/mainwin.h"
#include "qcr/base/debug.h"
#include <QApplication>
#include <QClipboard>
#include <QHeaderView>
#include <QKeyEvent>

//  ***********************************************************************************************
//! @class BigtableModel
//!
//! The first column contains row numbers. The remaining numCols columns contain data.

BigtableModel::BigtableModel()
    : TableModel("data#")
{
    gGui->bigtableModel = this; // for use in export dialog
    headers_ = PeakInfo::dataTags(false);
    comparators_ = PeakInfo::dataCmps();
    numCols_ = headers_.count();
    ASSERT((int)comparators_.size() == numCols_);
    colIndexMap_.resize(numCols_);
    for (int i=0; i<numCols_; ++i)
        colIndexMap_[i] = i;
}

void BigtableModel::refresh()
{
    if (!gSession->activeClusters.size() || !gSession->peaks.size())
        return;
    beginResetModel();
    rows_.clear();
    if (const InfoSequence* peakInfos = gSession->allPeaks.currentInfoSequence())
        for (const PeakInfo& r : peakInfos->peaks())
            rows_.push_back(XRow(rows_.size() + 1, r.data()));
    sortData();
    endResetModel();
}

QVariant BigtableModel::data(const QModelIndex& index, int role) const
{
    int indexRow = index.row(), indexCol = index.column();

    if (indexCol < 0 || indexRow < 0)
        return {};

    switch (role) {
    case Qt::DisplayRole: {
        if (0 == indexCol)
            return rows_.at(indexRow).n;
        const QVariant var = rows_.at(indexRow).row.at(indexCol-1);
        if (isNumeric(var) && qIsNaN(var.toDouble()))
            return {}; // show blank field instead of NAN
        return var;
    }
    case Qt::TextAlignmentRole: {
        if (0 == indexCol)
            return Qt::AlignRight;
        const QVariant& var = rows_.at(indexRow).row.at(indexCol-1);
        if (isNumeric(var))
            return Qt::AlignRight;
        return Qt::AlignLeft;
    }
    default: break;
    }

    return {};
}

QVariant BigtableModel::headerData(int section, Qt::Orientation, int role) const
{
    if (section < 0)
        return {};
    if (Qt::DisplayRole == role)
        return 0 == section ? "#" : headers_.at(section - 1);
    return {};
}

//! Called upon QHeaderView::sectionMoved.
void BigtableModel::onColumnMove(int from, int to)
{
    ASSERT(from < (int)colIndexMap_.size() && to < (int)colIndexMap_.size());
    qSwap(colIndexMap_[from], colIndexMap_[to]);
}

void BigtableModel::setSortColumn(int col)
{
    sortColumn_ = col < 0 ? col : colIndexMap_.at(col);
}

const std::vector<QVariant>& BigtableModel::row(int index) const
{
    return rows_.at(index).row;
}

void BigtableModel::sortData()
{
    auto _cmpRows = [this](int col,
                           const std::vector<QVariant>& r1, const std::vector<QVariant>& r2) {
        col = colIndexMap_.at(col);
        return comparators_.at(col)(r1.at(col), r2.at(col));
    };

    // sort by sortColumn first, then left-to-right
    auto _cmp = [this, _cmpRows](const XRow& r1, const XRow& r2) {
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

        for (int col=0; col<numCols_; ++col) {
            if (col == sortColumn_)
                continue;
            int c = _cmpRows(col, r1.row, r2.row);
            if (c < 0)
                return true;
            if (c > 0)
                return false;
        }
        return false;
    };

    beginResetModel();
    std::sort(rows_.begin(), rows_.end(), _cmp);
    endResetModel();
}

QStringList BigtableModel::getHeaders() const
{
    QStringList ret;
    const QStringList& headers = PeakInfo::dataTags(true);
    for (int i=0; i<headers.count(); ++i)
        if (gSession->params.bigMetaSelection.isSelected(i))
            ret.append(headers[i]);
    return ret;
}

std::vector<std::vector<const QVariant*>> BigtableModel::getData() const
{
    std::vector<std::vector<const QVariant*>> ret(rowCount());
    for (int i=0; i<rowCount(); ++i)
        for (int j=0; j<columnCount()-1; ++j)
            if (gSession->params.bigMetaSelection.isSelected(j))
                ret.at(i).push_back(&(rows_.at(i).row.at(j)));
    return ret;
}

//  ***********************************************************************************************
//!  @class BigtableView
//!

BigtableView::BigtableView()
    : TableView {new BigtableModel}
{
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

    // internal connections:
    connect(header(), &QHeaderView::sectionMoved,
            [this](int /*logicalIndex*/, int oldVisualIndex, int newVisualIndex) {
                ASSERT(oldVisualIndex > 0 && newVisualIndex > 0);
                header()->setSortIndicatorShown(false);
                model()->onColumnMove(oldVisualIndex-1, newVisualIndex-1);
                model()->sortData(); });

    connect(header(), &QHeaderView::sectionClicked, [this](int logicalIndex) {
        QHeaderView* h = header();
        h->setSortIndicatorShown(true);
        h->setSortIndicator(logicalIndex, Qt::AscendingOrder);
        model()->setSortColumn(logicalIndex-1);
        model()->sortData(); });
}

void BigtableView::refresh()
{
    model()->refresh();
}

void BigtableView::updateShownColumns()
{
    int nCol = model()->columnCount();
    for (int i=0; i<nCol-1; ++i) {
        if (gSession->params.bigMetaSelection.isSelected(i))
            showColumn(i + 1);
        else
            hideColumn(i + 1);
    }
}

//! To enable copying to external applications
void BigtableView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_C && event->modifiers() & Qt::ControlModifier)
        QApplication::clipboard()->setText(exportSelection());
    else
        QTreeView::keyPressEvent(event);
}

//! Encodes selected items as a string with separators '\t' and '\n', for use in keyPressEvent.
QString BigtableView::exportSelection() const
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
