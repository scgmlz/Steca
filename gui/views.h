// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/views.h
//! @brief     Defines ...
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

// A (tree-)list view with a reference to the hub. Single selection.

class ListView : public TreeListView, protected RefHub {
    SUPER(TreeListView) public : ListView(TheHub&);

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

// Multiple selection.

class MultiListView : public ListView {
    SUPER(ListView) public : MultiListView(TheHub&);

protected:
    void selectRows(uint_vec);
};
}
}
#endif // VIEWS_H
