//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/tab_bigtable.h
//! @brief     Defines class BigtableTab, and its dependences
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef TAB_BIGTABLE_H
#define TAB_BIGTABLE_H

#include <QWidget>

//! Tabular display of fit results and metadata, with associated controls.

class BigtableTab : public QWidget {
public:
    BigtableTab();
    const class BigtableView& bigtableView() const { return *bigtableView_; }
private:
    void render();
    void showEvent(QShowEvent*) { render(); }
    class BigtableView* bigtableView_;
};

#endif // TAB_BIGTABLE_H
