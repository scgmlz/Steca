// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/dock_files.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "dock_files.h"
#include "thehub.h"
#include "views.h"
#include <QHeaderView>

namespace gui {
namespace panel {

class FilesView : public views::MultiListView {
    CLASS(FilesView) SUPER(views::MultiListView) public : FilesView(TheHub&);

protected:
    using Model = models::FilesModel;
    Model* model() const { return static_cast<Model*>(super::model()); }

    void selectionChanged(QItemSelection const&, QItemSelection const&);
    void removeSelected();
    void recollect();
};

FilesView::FilesView(TheHub& hub) : super(hub) {
    setModel(&hub.filesModel);
    EXPECT(dynamic_cast<Model*>(super::model()))

    header()->hide();

    connect(hub_.actions.remFile, &QAction::triggered, [this]() { removeSelected(); });

    onSigFilesChanged([this]() {
        selectRows({});
        recollect();
    });

    onSigFilesSelected([this]() { selectRows(hub_.collectedFromFiles()); });
}

void FilesView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
    super::selectionChanged(selected, deselected);
    recollect();
}

void FilesView::removeSelected() {
    auto indexes = selectedIndexes();

    // backwards
    for (int i = indexes.count(); i-- > 0;)
        model()->remFile(to_u(indexes.at(i).row()));

    selectRows({});
    recollect();
}

void FilesView::recollect() {
    uint_vec rows;
    for (auto& index : selectionModel()->selectedRows())
        if (index.isValid())
            rows.append(to_u(index.row()));

    hub_.collectDatasetsFromFiles(rows);
}


DockFiles::DockFiles(TheHub& hub) : super("Files", "dock-files", Qt::Vertical), RefHub(hub) {
    auto& actions = hub_.actions;

    auto h = hbox();
    box_->addLayout(h);

    h->addStretch();
    h->addWidget(iconButton(actions.addFiles));
    h->addWidget(iconButton(actions.remFile));

    box_->addWidget((filesView_ = new FilesView(hub)));

    h = hbox();
    box_->addLayout(h);

    h->addWidget(label("Correction file"));

    h = hbox();
    box_->addLayout(h);

    h->addWidget((corrFile_ = new LineView()));
    h->addWidget(iconButton(actions.enableCorr));
    h->addWidget(iconButton(actions.remCorr));

    onSigCorrFile([this](data::shp_File file) {
        corrFile_->setText(file.isNull() ? EMPTY_STR : file->fileName());
    });
}


}
}
