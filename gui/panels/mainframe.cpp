//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/mainframe.cpp
//! @brief     Implements class Mainframe
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/panels/mainframe.h"
#include "core/session.h"
#include "gui/panels/tab_bigtable.h"
#include "gui/panels/tab_diagram.h"
#include "gui/panels/tab_image.h"
#include "gui/panels/tab_polefig.h"
//#include "qcr/base/debug.h"

//  ***********************************************************************************************
//! @class Mainframe

Mainframe::Mainframe()
    : QcrTabWidget {"mainTabs"}
{
    // layout
    setTabPosition(QTabWidget::North);
    addTab((dataImageTab_ = new DataImageTab), "Data image");
    addTab((corrImageTab_ = new CorrImageTab), "Corr image");
    addTab((bigtableTab_ = new BigtableTab), "Table");
    addTab((diagramTab_ = new DiagramTab), "Diagram");
    addTab((polefigTab_ = new PolefigTab), "Polefig");

    setRemake([this](){
            bool active = gSession->activeClusters.size();
            bool peakish = gSession->peaks.size();
            // Work against unwanted heuristics of QTabBar::setTabEnabled.
            // See https://bugreports.qt.io/browse/QTBUG-69930.
            //int oldCurrent = currentIndex();
            setTabEnabled(0, active);
            setTabEnabled(1, gSession->corrset.hasFile());
            setTabEnabled(2, active&&peakish);
            setTabEnabled(3, active&&peakish);
            setTabEnabled(4, active&&peakish);
            //setEnabled(anyEnabled());
            //if (!currentWidget()->isEnabled())
            //    programaticallySetValue(0);
            show(); });
}
