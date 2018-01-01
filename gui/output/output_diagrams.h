// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/output/output_diagrams.h
//! @brief     Defines class DiagramsFrame
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef OUTPUT_DIAGRAMS_H
#define OUTPUT_DIAGRAMS_H

#include "output_dialogs.h"

namespace gui {
namespace output {

class DiagramsFrame : public Frame {
private:
    using super = Frame;
public:
    DiagramsFrame(rcstr title, QWidget*);

protected:
    class TabPlot* tabPlot_;
    class TabDiagramsSave* tabSave_;

    using eReflAttr = calc::ReflectionInfo::eReflAttr;

    eReflAttr xAttr() const;
    eReflAttr yAttr() const;

    void displayReflection(uint reflIndex, bool interpolated);

    calc::ReflectionInfos rs_;
    qreal_vec xs_, ys_, ysErrorLo_, ysErrorUp_;

    void recalculate();

    void saveDiagramOutput();
    void writeCurrentDiagramOutputFile(rcstr filePath, rcstr separator);
    void writeAllDataOutputFile(rcstr filePath, rcstr separator);
};

} // namespace output
} // namespace gui

#endif // OUTPUT_DIAGRAMS_H
