// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_files.cpp
//! @brief     Implements class SubframeFiles, with local model and view
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

class FilesModel : public TableModel { // < QAbstractTableModel < QAbstractItemModel
public:
    void onClicked(const QModelIndex &);
    void onFilesChanged();
    void onHighlight();
    void onActivated();
    void removeFile();
    void setHighlight(int row);

    int columnCount() const final { return 3; }
    int rowCount() const final { return gSession->dataset().countFiles(); }
    QVariant data(const QModelIndex&, int) const final;

private:
    void setActivated(int row, Qt::CheckState state);
    void updateChecked();

    vec<Qt::CheckState> checked_;
    int rowHighlighted_;
};

//! Selects or unselects all measurements in a file.
void FilesModel::onClicked(const QModelIndex& cell) {
    int row = cell.row();
    if (row < 0 || row >= rowCount())
        return;
    int col = cell.column();
    if (col==1) {
        setActivated(row, checked_.at(row)==Qt::Checked ? Qt::Unchecked : Qt::Checked);
    }
}

void FilesModel::onFilesChanged() {
    beginResetModel();
    updateChecked();
    endResetModel();
    if (rowHighlighted_<0)
        setHighlight(0);
}

//! Update highlight display upon sigHighlight.
void FilesModel::onHighlight() {
    if (!gSession->dataset().countFiles())
        return;
    const Datafile* newFile = gSession->dataset().highlightedFile();
    if (!newFile)
        return;
    for (int row=0; row<rowCount(); ++row) {
        if (&gSession->dataset().file(row)==newFile) {
            setHighlight(row);
            return;
        }
    }
    NEVER
}

//! Update activation check display upon sigActivated.
void FilesModel::onActivated() {
    for (int row=0; row<rowCount(); ++row)
        setActivated(row, gSession->dataset().file(row).activated());
}

//! Forwards command to remove file, and updates the view.
void FilesModel::removeFile() {
    int row = rowHighlighted_;
    gHub->removeFile(row);
    updateChecked();
    emit dataChanged(createIndex(row,0),createIndex(rowCount(),columnCount()));
    setHighlight(qMin(row, rowCount()-1));
}

//! Sets rowHighlighted_, and signals need to refresh FilesView and MeasurementView.
void FilesModel::setHighlight(int row) {
    int oldRow = rowHighlighted_;
    if (row==oldRow)
        return;
    rowHighlighted_ = row;
    if (row>=0) {
        emit dataChanged(createIndex(row,0),createIndex(row,columnCount()));
        gSession->dataset().setHighlight(&gSession->dataset().file(row));
    }
    if (oldRow>=0)
        emit dataChanged(createIndex(oldRow,0),createIndex(oldRow,columnCount()));
}

//! Returns role-specific information about one table cell.
QVariant FilesModel::data(const QModelIndex& index, int role) const {
    const int row = index.row();
    if (row < 0 || row >= rowCount())
        return {};
    const Datafile& file = gSession->dataset().file(row);
    int col = index.column();
    switch (role) {
    case Qt::EditRole:
        return {};
    case Qt::DisplayRole:
        if (col==2)
            return file.name();
        return {};
    case Qt::ToolTipRole:
        if (col>=2)
            return QString("File %1\ncontains %2 measurements\nhere numbered %3 to %4")
                .arg(file.name())
                .arg(file.count())
                .arg(gSession->dataset().offset(file)+1)
                .arg(gSession->dataset().offset(file)+file.count());
        return {};
    case Qt::CheckStateRole: {
        if (col==1) {
            return checked_.at(row);
        }
        return {};
    }
    case Qt::BackgroundRole: {
        if (row==rowHighlighted_)
            return QColor(Qt::cyan);
        return QColor(Qt::white);
    }
    default:
        return {};
    }
}

void FilesModel::setActivated(int row, Qt::CheckState state) {
    checked_[row] = state;
    if (state==gSession->dataset().file(row).activated())
        return;
    emit dataChanged(createIndex(row,1),createIndex(row,1));
    //gSession->dataset().activateFile(row, state);
}

void FilesModel::updateChecked() {
    int oldsize = checked_.count();
    checked_.resize(rowCount());
    for (int row = oldsize; row<rowCount(); ++row)
        setActivated(row, gSession->dataset().file(row).activated());
}


// ************************************************************************** //
//  local class FilesView
// ************************************************************************** //

//! Main item in SubframeFiles: View and control the list of DataFile's

class FilesView : public ListView { // < QTreeView < QAbstractItemView
public:
    FilesView();

private:
    void currentChanged(QModelIndex const&, QModelIndex const&) override final;

    int sizeHintForColumn(int) const final;
    FilesModel* model() const { return static_cast<FilesModel*>(ListView::model()); }
};

FilesView::FilesView() : ListView() {
    setHeaderHidden(true);
    setSelectionMode(QAbstractItemView::NoSelection);
    auto filesModel = new FilesModel();
    setModel(filesModel);

    connect(gSession, &Session::sigFiles, model(), &FilesModel::onFilesChanged);
    connect(gHub->trigger_removeFile, &QAction::triggered, model(), &FilesModel::removeFile);
    connect(gSession, &Session::sigHighlight, model(), &FilesModel::onHighlight);
    connect(gSession, &Session::sigActivated, model(), &FilesModel::onActivated);
    connect(this, &FilesView::clicked, model(), &FilesModel::onClicked);
}

//! Overrides QAbstractItemView. This slot is called when a new item becomes the current item.
void FilesView::currentChanged(QModelIndex const& current, QModelIndex const& previous) {
    model()->setHighlight(current.row());
    scrollTo(current);
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
    h->addWidget(newQ::IconButton(gHub->trigger_corrFile));
    h->addWidget(newQ::IconButton(gHub->toggle_enableCorr));

    connect(gSession, &Session::sigCorr, [corrFile_]() {
            corrFile_->setText( gSession->corrset().hasFile() ?
                                gSession->corrset().raw().fileName() : ""); });
}
