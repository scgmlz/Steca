// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_files.cpp
//! @brief     Implements class SubframeFiles; defines and implements class FileViews
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/panels/subframe_files.h"
#include "core/session.h"
#include "gui/base/table_model.h"
#include "gui/thehub.h"
#include "gui/base/new_q.h"
#include "gui/base/tree_views.h" // inheriting from
#include <QHeaderView>


// ************************************************************************** //
//  local class FilesModel
// ************************************************************************** //

//! The model for FilesView

class FilesModel : public TableModel { // < TableModel < QAbstractTableModel < QAbstractItemModel
public:
    void removeFile(int i) { gHub->removeFile(i); }
    void onClicked(const QModelIndex &);

    int columnCount() const final { return 3; }
    int rowCount() const final { return gSession->numFiles(); }
    QVariant data(const QModelIndex&, int) const final;

    vec<bool> const& rowsChecked() const { return rowsChecked_; }
    int rowHighlighted() const { return rowHighlighted_; }

private:
    mutable vec<bool> rowsChecked_;
    int rowHighlighted_;
};


QVariant FilesModel::data(const QModelIndex& index, int role) const {
    const int row = index.row();
    if (row < 0 || row >= rowCount())
        return {};
    while (rowsChecked_.count()<rowCount())
        rowsChecked_.append(true);
    const Datafile* file = gSession->file(row);
    int col = index.column();
    switch (role) {
    case Qt::EditRole:
        return {};
    case Qt::DisplayRole:
        if (col==2)
            return file->fileName();
        return {};
    case Qt::UserRole:
        if (col==2)
            return QVariant::fromValue<const Datafile*>(file);
        return {};
    case Qt::ToolTipRole:
        if (col>=2)
            return QString("File %1\ncontains %2 measurements\nhere numbered %3 to %4")
                .arg(file->fileName())
                .arg(file->count())
                .arg(file->offset()+1)
                .arg(file->offset()+file->count());
        return {};
    case Qt::CheckStateRole: {
        if (col==1) {
            return rowsChecked_.at(row) ? Qt::Checked : Qt::Unchecked;
        }
        return {};
    }
    default:
        return {};
    }
}

void FilesModel::onClicked(const QModelIndex& cell) {
    int row = cell.row();
    if (row < 0 || row >= rowCount())
        return;
    int col = cell.column();
    if (col==1) {
        rowsChecked_[row] = !rowsChecked_[row];
    } else if (col==2) {
        qDebug() << "highlighted";
    }
    emit dataChanged(cell, cell);
}

// ************************************************************************** //
//  local class FilesView
// ************************************************************************** //

//! Main item in SubframeFiles: View and control the list of DataFile's

class FilesView : public ListView { // < TreeView < QTreeView < QAbstractItemView
public:
    FilesView();

private:
    int sizeHintForColumn(int) const final;

    void removeSelected();
    void recollect();

    FilesModel* model() const { return static_cast<FilesModel*>(ListView::model()); }
};

FilesView::FilesView() : ListView() {
    setHeaderHidden(true);
    setSelectionMode(QAbstractItemView::NoSelection);
    auto filesModel = new FilesModel();
    setModel(filesModel);

    connect(this, &FilesView::clicked, model(), &FilesModel::onClicked);
    connect(gHub, &TheHub::sigFilesChanged, [=]() { filesModel->signalReset(); });
    connect(gHub->trigger_removeFile, &QAction::triggered, [this]() { removeSelected(); });
    connect(gHub, &TheHub::sigFilesChanged, [this]() { selectRow({}); recollect(); });
// TODO    connect(gHub, &TheHub::sigFilesSelected, [this]() { selectRows(gSession->filesSelection()); });
}

void FilesView::removeSelected() {
    const QModelIndexList& indexes = selectedIndexes();
    // backwards
    for (int i = indexes.count(); i-- > 0;)
        model()->removeFile(indexes.at(i).row());
    selectRow({});
    recollect();
}

void FilesView::recollect() {
    vec<int> rows;
    for (const QModelIndex& index : selectionModel()->selectedRows())
        if (index.isValid())
            rows.append(index.row());
    gHub->collectDatasetsFromSelection(rows);
}

int FilesView::sizeHintForColumn(int col) const {
    switch (col) {
    case 1: {
        return 2*mWidth();
    } default:
        return 10*mWidth();
    }
}


// ************************************************************************** //
//  class SubframeFiles
// ************************************************************************** //

SubframeFiles::SubframeFiles() : DockWidget("Files", "dock-files") {

    auto h = newQ::HBoxLayout();
    box_->addLayout(h);

    h->addStretch();
    h->addWidget(newQ::IconButton(gHub->trigger_addFiles));
    h->addWidget(newQ::IconButton(gHub->trigger_removeFile));

    box_->addWidget(new FilesView());

    h = newQ::HBoxLayout();
    box_->addLayout(h);

    h->addWidget(newQ::Label("Correction file"));

    h = newQ::HBoxLayout();
    box_->addLayout(h);

    auto* corrFile_ = new QLineEdit();
    corrFile_->setReadOnly(true);
    h->addWidget(corrFile_);
    h->addWidget(newQ::IconButton(gHub->toggle_enableCorr));
    h->addWidget(newQ::IconButton(gHub->trigger_removeCorr));

    connect(gHub, &TheHub::sigCorrFile,
            [corrFile_](const Datafile* file) {
                corrFile_->setText(file ? file->fileName() : ""); });
}
