// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/output/output_diagrams.h
//! @brief     Defines class DiagramsFrame
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef OUTPUT_DIAGRAMS_H
#define OUTPUT_DIAGRAMS_H

#include "frame.h"




class DiagramsFrame : public Frame {
public:
    DiagramsFrame(rcstr title, QWidget*);

protected:
    class TabPlot* tabPlot_;
    class TabDiagramsSave* tabSave_;

    using eReflAttr = ReflectionInfo::eReflAttr;

    eReflAttr xAttr() const;
    eReflAttr yAttr() const;

    void displayReflection(uint reflIndex, bool interpolated);

    ReflectionInfos rs_;
    qreal_vec xs_, ys_, ysErrorLo_, ysErrorUp_;

    void recalculate();

    void saveDiagramOutput();
    void writeCurrentDiagramOutputFile(rcstr filePath, rcstr separator);
    void writeAllDataOutputFile(rcstr filePath, rcstr separator);
};




#endif // OUTPUT_DIAGRAMS_H
