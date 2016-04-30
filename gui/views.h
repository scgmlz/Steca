// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      views.h
//! @brief     Data views.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef VIEWS_H
#define VIEWS_H

#include "models.h"
#include "refhub.h"
#include "gui_helpers.h"

namespace gui { namespace views {
//------------------------------------------------------------------------------
/// A (tree-)list view with a reference to the hub. Single selection.

class ListView: public TreeListView, protected RefHub {
  SUPER(ListView,TreeListView)
public:
  ListView(TheHub&);

protected:
  using Model = models::TableModel;
  void updateSingleSelection();
  void selectRow(uint);
};

//------------------------------------------------------------------------------
/// Multiple selection.

class MultiListView: public ListView {
  SUPER(MultiListView,ListView)
public:
  MultiListView(TheHub&);

protected:
  using Model = models::TableModel;
  void selectRows(uint_vec);
};

//------------------------------------------------------------------------------
}}
#endif // VIEWS_H
