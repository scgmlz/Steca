// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      panel_file.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "panel_file.h"
#include "thehub.h"
#include "models.h"
#include <QStyledItemDelegate>
#include <QHeaderView>

namespace gui { namespace panel {
//------------------------------------------------------------------------------

FilesView::FilesView(TheHub& hub): super(hub)
, model_(hub.fileViewModel), selfSignal_(false) {
  setModel(&model_);
  setSelectionMode(ExtendedSelection);
  header()->hide();

  connect(hub_.actions.remFile, &QAction::triggered, [this]() {
    removeSelected();
    update();
  });

  ON_HUB_SIGNAL(filesChanged,  () { 
    clearSelection();
    update();
  })
  ON_HUB_SIGNAL(filesSelected, () {
    if (!selfSignal_)
      selectRows(hub_.collectedFromFiles());
  })
}

class bool_lock {
public:
  bool_lock(bool &val): val_(val) {
    val_ = true;
  }

 ~bool_lock() {
    val_ = false;
  }

private:
  bool &val_;
};

void FilesView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);

  uint_vec fileNums;
  for (auto &index: selectedIndexes()) {
    ASSERT(0 <= index.row());
    fileNums.append(index.row());
  }

  bool_lock __(selfSignal_);

  hub_.collectDatasetsFromFiles(fileNums);
  hub_.tellFilesSelectedDatasetsChanged();
}

void FilesView::removeSelected() {
  auto indexes = selectedIndexes();

  // backwards
  for (uint i = indexes.count(); i-- > 0; )
    model_.remFile(indexes.at(i).row());

  clearSelection();
  model_.signalReset();
}

void FilesView::update() {
  auto index = currentIndex();
  model_.signalReset();
  // keep the current index, or select the first item
  setCurrentIndex(index.isValid() ? index : model_.index(0,1));
}

//------------------------------------------------------------------------------

DockFiles::DockFiles(TheHub& hub)
: super("Files","dock-files",Qt::Vertical), RefHub(hub) {
  box_->addWidget((filesView_ = new FilesView(hub)));

  auto h = hbox(); box_->addLayout(h);

  auto &actions = hub_.actions;

  h->addWidget(label("Corr. file"));
  h->addStretch();
  h->addWidget(iconButton(actions.addFiles));
  h->addWidget(iconButton(actions.remFile));

  h = hbox(); box_->addLayout(h);

  h->addWidget((corrFile_  = new LineView()));
  h->addWidget(iconButton(actions.enableCorr));
  h->addWidget(iconButton(actions.remCorr));

  ON_HUB_SIGNAL(corrFileName, (rcstr fileName) { corrFile_->setText(fileName); })
}

//------------------------------------------------------------------------------
}}
// eof
