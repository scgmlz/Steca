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

class FilesModel : public TableModel {
public:
    void removeFile(uint i) { gHub->removeFile(i); }

    int columnCount() const final { return 2; }
    int rowCount() const final { return to_i(gSession->numFiles()); }
    QVariant data(const QModelIndex&, int) const;

    enum { GetFileRole = Qt::UserRole };
};


QVariant FilesModel::data(const QModelIndex& index, int role) const {
    const int row = index.row(), rowCnt = rowCount();
    if (row < 0 || rowCnt <= row)
        return {};

    switch (role) {
    case Qt::DisplayRole:
        return gSession->file(to_u(row))->fileName();
    case GetFileRole:
        return QVariant::fromValue<shp_Datafile>(gSession->file(to_u(row)));
    default:
        return {};
    }
}


// ************************************************************************** //
//  local class FilesView
// ************************************************************************** //

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
    connect(gHub, &TheHub::sigFilesSelected,
            [this]() { selectRows(gSession->collectedFromFiles()); });
}

void FilesView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
    MultiListView::selectionChanged(selected, deselected);
    recollect();
}

void FilesView::removeSelected() {
    const QModelIndexList& indexes = selectedIndexes();
    // backwards
    for (int i = indexes.count(); i-- > 0;)
        model()->removeFile(to_u(indexes.at(i).row()));
    selectRows({});
    recollect();
}

void FilesView::recollect() {
    uint_vec rows;
    for (const QModelIndex& index : selectionModel()->selectedRows())
        if (index.isValid())
            rows.append(to_u(index.row()));
    gHub->collectDatasetsFromFiles(rows);
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
    h->addWidget(newQ::IconButton(gHub->trigger_remCorr));

    connect(gHub, &TheHub::sigCorrFile,
            [corrFile_](shp_Datafile file) {
                corrFile_->setText(file.isNull() ? "" : file->fileName()); });
}
