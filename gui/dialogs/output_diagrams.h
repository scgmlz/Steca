// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/output_diagrams.h
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

#include "gui/dialogs/frame.h"

//! The modal dialog for viewing and saving diagrams
class DiagramsFrame : public Frame {
public:
    DiagramsFrame();
    ~DiagramsFrame();

private:
    class TabPlot* tabPlot_;
    class TabDiagramsSave* tabSave_;

    using eReflAttr = PeakInfo::eReflAttr;

    eReflAttr xAttr() const;
    eReflAttr yAttr() const;

    void displayPeak(int reflIndex, bool interpolated);

    PeakInfos rs_;
    vec<qreal> xs_, ys_, ysErrorLo_, ysErrorUp_;

    void recalculate();

    void saveDiagramOutput();
    void writeCurrentDiagramOutputFile(const QString& filePath, const QString& separator);
    void writeAllDataOutputFile(const QString& filePath, const QString& separator);
};

#endif // OUTPUT_DIAGRAMS_H
