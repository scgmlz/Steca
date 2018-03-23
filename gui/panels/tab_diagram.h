// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/tab_diagram.h
//! @brief     Defines class DiagramWidget, and its dependences
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TAB_DIAGRAM_H
#define TAB_DIAGRAM_H

#include "core/data/peak_info.h"
#include "gui/mainwin.h"

//! Plot of a pair of fit results or metadata, with associated controls.

class DiagramWidget : public QWidget {
public:
    DiagramWidget();
    void render();
private:
    class PlotDiagram* plot_;
    class SelectXY* selectXY_;

    using eReflAttr = PeakInfo::eReflAttr;

    eReflAttr xAttr() const;
    eReflAttr yAttr() const;

    void displayPeak(int reflIndex, bool interpolated);

    PeakInfos rs_;
    QVector<qreal> xs_, ys_, ysErrorLo_, ysErrorUp_;
};

#endif // TAB_DIAGRAM_H
