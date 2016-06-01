// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      views.h
//! @brief     Data views.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Antti Soininen, Jan Burle, Rebecca Brydon
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef VIEWS_H
#define VIEWS_H

#include "gui_helpers.h"
#include "models.h"
#include "refhub.h"

namespace gui { namespace views {
//------------------------------------------------------------------------------
/// A (tree-)list view with a reference to the hub. Single selection.

class ListView : public TreeListView, protected RefHub {
  SUPER(ListView, TreeListView)
public:
  ListView(TheHub&);

  using Model = models::TableModel;

  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Woverloaded-virtual"
  void setModel(Model*);
  #pragma GCC diagnostic pop

protected:
  Model* model() const { return static_cast<Model*>(super::model()); }

  void updateSingleSelection();
  void selectRow(uint);
};

//------------------------------------------------------------------------------
/// Multiple selection.

class MultiListView : public ListView {
  SUPER(MultiListView, ListView)
public:
  MultiListView(TheHub&);

protected:
  void selectRows(uint_vec);
};

//------------------------------------------------------------------------------
}}
#endif  // VIEWS_H
