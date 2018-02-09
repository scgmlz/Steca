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
#include "gui/base/model_view.h"
#include "gui/base/controls.h"
#include "gui/mainwin.h"


// ************************************************************************** //
//  local class FilesModel
// ************************************************************************** //

//! The model for FilesView

class FilesModel : public CheckTableModel { // < QAbstractTableModel < QAbstractItemModel
public:
    FilesModel() : CheckTableModel("file") {}
    int highlighted() const final { return gSession->dataset().highlight().fileIndex(); }
    void setHighlight(int i) final { gSession->dataset().highlight().setFile(i); }
    bool activated(int i) const { return gSession->dataset().fileAt(i).activated(); }
    void setActivated(int i, bool on) { gSession->dataset().setFileActivation(i, on); }

private:
    int columnCount() const final { return 3; }
    int rowCount() const final { return gSession->dataset().countFiles(); }
    QVariant data(const QModelIndex&, int) const final;
};

//! Returns role-specific information about one table cell.
QVariant FilesModel::data(const QModelIndex& index, int role) const {
    const int row = index.row();
    if (row < 0 || row >= rowCount())
        return {};
    const Datafile& file = gSession->dataset().fileAt(row);
    int col = index.column();
    if (role==Qt::DisplayRole && col==2)
        return file.name();
    else if (role==Qt::ToolTipRole && col>=2)
        return QString("File %1\ncontains %2 measurements.here numbered %3 to %4")
            .arg(file.name())
            .arg(file.count())
            .arg(gSession->dataset().offset(file)+1)
            .arg(gSession->dataset().offset(file)+file.count());
    else if (role==Qt::CheckStateRole && col==1)
        return activated(row);
    else if (role==Qt::BackgroundRole) {
        if (row==highlighted())
            return QColor(Qt::cyan);
        return QColor(Qt::white);
    }
    return {};
}


// ************************************************************************** //
//  local class FilesView
// ************************************************************************** //

//! Main item in SubframeFiles: View and control the list of DataFile's

class FilesView : public CheckTableView {
public:
    FilesView();

private:
    void currentChanged(QModelIndex const& current, QModelIndex const&) override final {
        gotoCurrent(current); }
    int sizeHintForColumn(int) const final;
    FilesModel* model() { return static_cast<FilesModel*>(model_); }
};

FilesView::FilesView()
    : CheckTableView(new FilesModel())
{
    connect(gSession, &Session::sigFiles, this, &TableView::onData);
    connect(gSession, &Session::sigDataHighlight, this, &TableView::onHighlight);
    connect(gSession, &Session::sigActivated, this, &CheckTableView::onActivated);
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
