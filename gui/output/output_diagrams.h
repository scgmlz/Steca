// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/output/output_diagrams.h
//! @brief     Defines class DiagramsFrame
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef OUTPUT_DIAGRAMS_H
#define OUTPUT_DIAGRAMS_H

#include "gui/output/frame.h"

//! The modal dialog for viewing and saving diagrams
class DiagramsFrame : public Frame {
public:
    DiagramsFrame(rcstr title, QWidget*);

private:
    class TabPlot* tabPlot_;
    class TabDiagramsSave* tabSave_;

    using eReflAttr = ReflectionInfo::eReflAttr;

    eReflAttr xAttr() const;
    eReflAttr yAttr() const;

    void displayReflection(int reflIndex, bool interpolated);

    ReflectionInfos rs_;
    vec<qreal> xs_, ys_, ysErrorLo_, ysErrorUp_;

    void recalculate();

    void saveDiagramOutput();
    void writeCurrentDiagramOutputFile(rcstr filePath, rcstr separator) const;
    void writeAllDataOutputFile(rcstr filePath, rcstr separator) const;
};

#endif // OUTPUT_DIAGRAMS_H
