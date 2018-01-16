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

class FilesModel : public TableModel {
public:
    void removeFile(int i) { gHub->removeFile(i); }

    int columnCount() const final { return 2; }
    int rowCount() const final { return gSession->numFiles(); }
    QVariant data(const QModelIndex&, int) const;
};


QVariant FilesModel::data(const QModelIndex& index, int role) const {
    const int row = index.row(), rowCnt = rowCount();
    if (row < 0 || rowCnt <= row)
        return {};
    const Datafile* file = gSession->file(row);
    switch (role) {
    case Qt::DisplayRole:
        return file->fileName();
    case Qt::UserRole:
        return QVariant::fromValue<const Datafile*>(file);
    case Qt::ToolTipRole:
        return QString("File %1\ncontains %2 measurements\nhere numbered %3 to %4")
            .arg(file->fileName())
            .arg(file->count())
            .arg(file->offset()+1)
            .arg(file->offset()+file->count());
    default:
        return {};
    }
}


// ************************************************************************** //
//  local class FilesView
// ************************************************************************** //

//! Main item in SubframeFiles: View and control the list of DataFile's

class FilesView : public MultiListView {
public:
    FilesView();

private:
    void selectionChanged(QItemSelection const&, QItemSelection const&);
    void removeSelected();
    void recollect();

    FilesModel* model() const { return static_cast<FilesModel*>(MultiListView::model()); }
};

FilesView::FilesView() : MultiListView() {
    auto filesModel = new FilesModel();
    setModel(filesModel);
    connect(gHub, &TheHub::sigFilesChanged, [=]() { filesModel->signalReset(); });
    debug::ensure(dynamic_cast<FilesModel*>(MultiListView::model()));

    header()->hide();

    connect(gHub->trigger_removeFile, &QAction::triggered, [this]() { removeSelected(); });
    connect(gHub, &TheHub::sigFilesChanged, [this]() { selectRows({}); recollect(); });
    connect(gHub, &TheHub::sigFilesSelected, [this]() { selectRows(gSession->filesSelection()); });
}

void FilesView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
    MultiListView::selectionChanged(selected, deselected);
    recollect();
}

void FilesView::removeSelected() {
    const QModelIndexList& indexes = selectedIndexes();
    // backwards
    for (int i = indexes.count(); i-- > 0;)
        model()->removeFile(indexes.at(i).row());
    selectRows({});
    recollect();
}

void FilesView::recollect() {
    vec<int> rows;
    for (const QModelIndex& index : selectionModel()->selectedRows())
        if (index.isValid())
            rows.append(index.row());
    gHub->collectDatasetsFromSelection(rows);
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
