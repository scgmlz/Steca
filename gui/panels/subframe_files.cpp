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
#include "qcr/widgets/controls.h"
#include "qcr/widgets/tables.h"
#include "core/session.h"
#include "gui/view/toggles.h"
#include "gui/actions/triggers.h"
#include "gui/mainwin.h"


//  ***********************************************************************************************
//! @class FilesModel (local scope)

//! The model for FilesView

class FilesModel : public CheckTableModel { // < QAbstractTableModel < QAbstractItemModel
public:
    FilesModel() : CheckTableModel{"datafiles"} {}

private:
    void setActivated(int i, bool on) final { gSession->dataset.fileAt(i).setFileActivation(on); }

    int highlighted() const final;
    void onHighlight(int i) final { gSession->dataset.highlight().setFile(i); }
    bool activated(int i) const final { return gSession->dataset.fileAt(i).activated(); }
    Qt::CheckState state(int i) const final {
        return gSession->dataset.fileAt(i).clusterState(); }

    int columnCount() const final { return 3; }
    int rowCount() const final { return gSession->dataset.countFiles(); }
    QVariant entry(int, int) const final;
    QString tooltip(int, int) const final;
};

int FilesModel::highlighted() const
{
    const Cluster* c = gSession->currentCluster();
    return c ? c->file().index() : -1;
}

QVariant FilesModel::entry(int row, int col) const
{
    const Datafile& file = gSession->dataset.fileAt(row);
    if(col == 2)
        return file.name();
    return {};
}

QString FilesModel::tooltip(int row, int col) const
{
    const Datafile& file = gSession->dataset.fileAt(row);
    if(col == 2)
        return QString("File %1\ncontains %2 measurements.here numbered %3 to %4")
            .arg(file.name())
            .arg(file.numMeasurements())
            .arg(gSession->dataset.offset(file)+1)
            .arg(gSession->dataset.offset(file)+file.numMeasurements());
    return {};
}


//  ***********************************************************************************************
//! @class FilesView (local scope)

//! Main item in `SubframeFiles`: View and control the list of `DataFile`s

class FilesView : public CheckTableView {
public:
    FilesView() : CheckTableView {new FilesModel} {}
private:
    FilesModel* model() { return static_cast<FilesModel*>(model_); }
};


//  ***********************************************************************************************
//! @class SubframeFiles

SubframeFiles::SubframeFiles()
    : QcrDockWidget{"files"}
{
    setFeatures(DockWidgetMovable);
    setWindowTitle("Files");

    setWidget(new QWidget);

    auto* dataControls = new QHBoxLayout;
    dataControls->addStretch();
    dataControls->addWidget(new QcrIconTriggerButton{&gGui->triggers->addFiles});
    dataControls->addWidget(new QcrIconTriggerButton{&gGui->triggers->removeFile});

    auto* corrFileView = new QcrLineDisplay{[]()->QString {
            return gSession->corrset.fileName(); }};

    auto* corrControls = new QHBoxLayout;
    corrControls->addWidget(corrFileView);
    corrControls->addWidget(new QcrIconTriggerButton{&gGui->triggers->corrFile});
    corrControls->addWidget(new QcrIconToggleButton{&gGui->toggles->enableCorr});

    auto* box = new QVBoxLayout;
    box->addLayout(dataControls);
    box->addWidget(new FilesView);
    box->addWidget(new QLabel{"Correction file"});
    box->addLayout(corrControls);
    box->setContentsMargins(0,0,0,0);
    widget()->setLayout(box);
}
