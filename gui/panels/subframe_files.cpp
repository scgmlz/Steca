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
#include "gui/base/layout.h"
#include "gui/base/model_view.h"
#include "gui/base/controls.h"
#include "gui/mainwin.h"
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

private:
    int columnCount() const final { return 3; }
    int rowCount() const final { return gSession->dataset().countFiles(); }
    QVariant data(const QModelIndex&, int) const final;
};

//! Selects or unselects all measurements in a file.
void FilesModel::onClicked(const QModelIndex& cell) {
    int row = cell.row();
    int col = cell.column();
    if (row < 0 || row >= rowCount())
        return;
    if (col==1)
        gSession->dataset().cycleFileActivation(row);
    else if (col==2)
        gSession->dataset().highlight().setFile(row);
}

void FilesModel::onFilesChanged() {
    resetModel(); // repaint everything, and reset currentIndex to origin
}

//! Update highlight display upon sigHighlight.
void FilesModel::onHighlight() {
    emit dataChanged(createIndex(0,0),createIndex(rowCount()-1,columnCount()-1));
}

//! Update activation check display upon sigActivated.
void FilesModel::onActivated() {
    emit dataChanged(createIndex(0,1),createIndex(rowCount()-1,1));
}

//! Returns role-specific information about one table cell.
QVariant FilesModel::data(const QModelIndex& index, int role) const {
    const int row = index.row();
    if (row < 0 || row >= rowCount())
        return {};
    const Datafile& file = gSession->dataset().fileAt(row);
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
            return QString("File %1\ncontains %2 measurements.here numbered %3 to %4")
                .arg(file.name())
                .arg(file.count())
                .arg(gSession->dataset().offset(file)+1)
                .arg(gSession->dataset().offset(file)+file.count());
        return {};
    case Qt::CheckStateRole: {
        if (col==1)
            return file.activated();
        return {};
    }
    case Qt::BackgroundRole: {
        if (row==gSession->dataset().highlight().fileIndex())
            return QColor(Qt::cyan);
        return QColor(Qt::white);
    }
    default:
        return {};
    }
}


// ************************************************************************** //
//  local class FilesView
// ************************************************************************** //

//! Main item in SubframeFiles: View and control the list of DataFile's

class FilesView : public TableView { // < QTreeView < QAbstractItemView
public:
    FilesView();

private:
    void currentChanged(QModelIndex const& current, QModelIndex const&) override final {
        gotoCurrent(current); }
    int sizeHintForColumn(int) const final;
    FilesModel* model() { return static_cast<FilesModel*>(model_); }
    // interaction with data
    int data_highlighted() final { return gSession->dataset().highlight().fileIndex(); }
    void data_setHighlight(int i) final { gSession->dataset().highlight().setFile(i); }
};

FilesView::FilesView()
    : TableView("file", new FilesModel())
{
    setHeaderHidden(true);

    connect(gSession, &Session::sigFiles, model(), &FilesModel::onFilesChanged);
    connect(gSession, &Session::sigHighlight, model(), &FilesModel::onHighlight);
    connect(gSession, &Session::sigActivated, model(), &FilesModel::onActivated);
    connect(this, &FilesView::clicked, model(), &FilesModel::onClicked);
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
    h->addWidget(new XIconButton(gGui->trigger_addFiles));
    h->addWidget(new XIconButton(gGui->trigger_removeFile));

    box_->addWidget(new FilesView());

    h = newQ::HBoxLayout();
    box_->addLayout(h);

    h->addWidget(new QLabel("Correction file"));

    h = newQ::HBoxLayout();
    box_->addLayout(h);

    auto* corrFile_ = new QLineEdit();
    corrFile_->setReadOnly(true);
    h->addWidget(corrFile_);
    h->addWidget(new XIconButton(gGui->trigger_corrFile));
    h->addWidget(new XIconButton(gGui->toggle_enableCorr));

    connect(gSession, &Session::sigCorr, [corrFile_]() {
            corrFile_->setText( gSession->corrset().hasFile() ?
                                gSession->corrset().raw().fileName() : ""); });
}
