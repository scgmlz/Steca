/*******************************************************************************
 * REVIEW: STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#ifndef VIEWS_H
#define VIEWS_H

#include "gui_helpers.h"
#include "models.h"
#include "refhub.h"
#include <QItemDelegate>

namespace gui { namespace views {
//------------------------------------------------------------------------------
// A (tree-)list view with a reference to the hub. Single selection.

class ListView : public TreeListView, protected RefHub {
  CLASS(ListView) SUPER(TreeListView)
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
  void selectRow(int);
};

//------------------------------------------------------------------------------
// Multiple selection.

class MultiListView : public ListView {
  CLASS(MultiListView) SUPER(ListView)
public:
  MultiListView(TheHub&);

protected:
  void selectRows(uint_vec);
};

//------------------------------------------------------------------------------
}}
#endif // VIEWS_H
