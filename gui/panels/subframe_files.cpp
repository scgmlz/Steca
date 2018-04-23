//  ***********************************************************************************************
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
//  ***********************************************************************************************

#include "gui/panels/subframe_files.h"
#include "core/session.h"
#include "qcr/widgets/displays.h"
#include "qcr/widgets/model_view.h"
#include "gui/mainwin.h"
#include "gui/actions/toggles.h"
#include "gui/actions/triggers.h"


//  ***********************************************************************************************
//! @class FilesModel (local scope)

//! The model for FilesView

class FilesModel : public CheckTableModel { // < QAbstractTableModel < QAbstractItemModel
public:
    FilesModel() : CheckTableModel("file") {}
    int highlighted() const final { return gSession->dataset().highlight().fileIndex(); }
    void setHighlight(int i) final { gSession->dataset().highlight().setFile(i); }
    bool activated(int i) const { return gSession->dataset().fileAt(i).activated() == Qt::Checked; }
    void setActivated(int i, bool on) { gSession->dataset().setFileActivation(i, on); }
private:
    int columnCount() const final { return 3; }
    int rowCount() const final { return gSession->dataset().countFiles(); }
    QVariant data(const QModelIndex&, int) const final;
};

//! Returns role-specific information about one table cell.
QVariant FilesModel::data(const QModelIndex& index, int role) const
{
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
            .arg(file.numMeasurements())
            .arg(gSession->dataset().offset(file)+1)
            .arg(gSession->dataset().offset(file)+file.numMeasurements());
    else if (role==Qt::CheckStateRole && col==1)
        return file.activated();
    else if (role==Qt::BackgroundRole) {
        if (row==highlighted())
            return QColor(Qt::cyan);
        return QColor(Qt::white);
    }
    return {};
}


//  ***********************************************************************************************
//! @class FilesView (local scope)

//! Main item in SubframeFiles: View and control the list of DataFile's

class FilesView : public CheckTableView {
public:
    FilesView();
private:
    void currentChanged(const QModelIndex& current, const QModelIndex&) override final {
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

int FilesView::sizeHintForColumn(int col) const
{
    switch (col) {
    case 1: {
        return 2*mWidth();
    } default:
        return 10*mWidth();
    }
}


//  ***********************************************************************************************
//! @class SubframeFiles

SubframeFiles::SubframeFiles()
{
    setFeatures(DockWidgetMovable);
    setWindowTitle("Files");
    setObjectName("dock-files");

    // layout
    setWidget(new QWidget);

    auto* dataControls = new QHBoxLayout;
    dataControls->addStretch();
    dataControls->addWidget(new QcrIconButton(&gGui->triggers->addFiles));
    dataControls->addWidget(new QcrIconButton(&gGui->triggers->removeFile));

    auto* corrFileView = new XLineDisplay;

    auto* corrControls = new QHBoxLayout;
    corrControls->addWidget(corrFileView);
    corrControls->addWidget(new QcrIconButton(&gGui->triggers->corrFile));
    corrControls->addWidget(new QcrIconButton(&gGui->toggles->enableCorr));

    auto* box = new QVBoxLayout;
    box->addLayout(dataControls);
    box->addWidget(new FilesView());
    box->addWidget(new QLabel("Correction file"));
    box->addLayout(corrControls);
    box->setContentsMargins(0,0,0,0);
    widget()->setLayout(box);

    // inbound connection
    connect(gSession, &Session::sigCorr, [corrFileView]() {
            corrFileView->setText( gSession->corrset().hasFile() ?
                                   gSession->corrset().raw().fileName() : ""); });
}
