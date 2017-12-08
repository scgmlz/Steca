// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/output/output_diagrams.h
//! @brief     Defines ...
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
#include "qcustomplot.h"
#include "typ/typ_str.h"

namespace gui {
namespace output {

class TabPlot : public QCustomPlot {
private:
    using super = QCustomPlot;
public:
    TabPlot();
    void set(calc::ReflectionInfos);

    void plot(qreal_vec const& xs, qreal_vec const& ys, qreal_vec const& ysLo, qreal_vec const& ysUp);

protected:
    QCPGraph *graph_, *graphLo_, *graphUp_;
};

class TabDiagramsSave : public TabSave {
private:
    using super = TabSave;
public:
    TabDiagramsSave(TheHub&, Params&);

    uint currType() const;
    bool currDiagram() const;

protected:
    QRadioButton *currentDiagram_, *allData_;
    QComboBox* fileTypes_;
};

class DiagramsFrame : public Frame {
private:
    using super = Frame;
public:
    DiagramsFrame(TheHub&, rcstr title, QWidget*);

protected:
    TabPlot* tabPlot_;
    TabDiagramsSave* tabSave_;

    using eReflAttr = calc::ReflectionInfo::eReflAttr;

    eReflAttr xAttr() const;
    eReflAttr yAttr() const;

    void displayReflection(uint reflIndex, bool interpolated);

    calc::ReflectionInfos rs_;
    qreal_vec xs_, ys_, ysErrorLo_, ysErrorUp_;

    void recalculate();

    bool saveDiagramOutput();
    void writeCurrentDiagramOutputFile(rcstr filePath, rcstr separator);
    void writeAllDataOutputFile(rcstr filePath, rcstr separator);
};
}
}
#endif // OUTPUT_DIAGRAMS_H
