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

namespace gui { namespace panel {
//------------------------------------------------------------------------------

DockFiles::DockFiles(TheHub& hub)
: super("Files","dock-files",Qt::Vertical), RefHub(hub) {
  box_->addWidget((filesView_ = new views::FilesView(hub)));

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

  ON_CORR_FILENAME([this](rcstr fileName) { corrFile_->setText(fileName); });
}

//------------------------------------------------------------------------------
}}
// eof
