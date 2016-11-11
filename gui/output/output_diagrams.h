// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      output_diagrams.h
//! @brief     Output of diagrams.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle, Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef OUTPUT_DIAGRAMS_H
#define OUTPUT_DIAGRAMS_H

#include "output_dialogs.h"
#include "QCP/qcustomplot.h"

namespace gui { namespace output {
//------------------------------------------------------------------------------

class TabPlot : public QCustomPlot {
  CLS(TabPlot) SUPER(QCustomPlot)
public:
  TabPlot();
  void set(calc::ReflectionInfos);

  void plot(qreal_vec::rc xs,
            qreal_vec::rc ys, qreal_vec::rc ysLo, qreal_vec::rc ysUp);

protected:
  QCPGraph *graph_, *graphLo_, *graphUp_;
};

//------------------------------------------------------------------------------

class TabDiagramsSave : public TabSave {
  CLS(TabDiagramsSave) SUPER(TabSave)
public:
  TabDiagramsSave(TheHub&, Params&);

  uint currType() const;
  bool currDiagram() const;

protected:
  QRadioButton *currentDiagram_, *allData_;
  QComboBox    *fileTypes_;
};

//------------------------------------------------------------------------------

class DiagramsFrame : public Frame {
  CLS(DiagramsFrame) SUPER(Frame)
public:
  DiagramsFrame(TheHub&, rcstr title, QWidget*);

protected:
  TabPlot         *tabPlot_;
  TabDiagramsSave *tabSave_;

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

//------------------------------------------------------------------------------
}}
#endif // OUTPUT_DIAGRAMS_H
