// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/views.h
//! @brief     Defines classes ListView, MultiListView
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef VIEWS_H
#define VIEWS_H

#include "gui_helpers.h"
#include "models.h"

namespace gui {
namespace views {

//! A (tree-)list view with a reference to the hub. Single selection.

class ListView : public TreeListView, protected RefHub {
public:
    ListView(TheHub&);

    using Model = models::TableModel;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
    // base class has setModel(<other type>*)
    void setModel(Model* model) { TreeListView::setModel(model); }
#pragma GCC diagnostic pop

protected:
    Model* model() const { return static_cast<Model*>(TreeListView::model()); }
    void updateSingleSelection();
    void selectRow(int);
};


//! A (tree-)list view with a reference to the hub. Multiple selection.

class MultiListView : public ListView {
public:
    MultiListView(TheHub&);

protected:
    void selectRows(uint_vec);
};

} // namespace views
} // namespace gui

#endif // VIEWS_H
