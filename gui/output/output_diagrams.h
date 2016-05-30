// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      output_diagrams.h
//! @brief     Output of diagrams.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef OUTPUT_DIAGRAMS_H
#define OUTPUT_DIAGRAMS_H

#include "output_dialogs.h"
#include "QCP/qcustomplot.h"

namespace gui { namespace output {
//------------------------------------------------------------------------------

class DiagramsParams : public Params {
  SUPER(DiagramsParams, Params)
public:
  DiagramsParams(TheHub&);

protected:
  panel::GridPanel *gpAxes_;
public:
  QComboBox *xAxis, *yAxis;
};

//------------------------------------------------------------------------------

class TabPlot : public QCustomPlot {
  SUPER(TabPlot, QCustomPlot)
public:
  TabPlot();
  void set(core::ReflectionInfos);

  void plot(uint xIndex, uint yIndex);

protected:
  core::ReflectionInfos rs_;
  QCPGraph *graph_;
};

//------------------------------------------------------------------------------

class TabDiagramsSave : public TabSave {
  SUPER(TabDiagramsSave, TabSave)
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
  SUPER(DiagramsFrame, Frame)
public:
  DiagramsFrame(TheHub&, rcstr title, QWidget*);

protected:
  TabPlot         *tabPlot_;
  TabDiagramsSave *tabSave_;

  void displayReflection(uint reflIndex, bool interpolated);

  void plot();
  void calculateErrors(uint yIndex, uint count, qreal_vec xs, qreal_vec ys);

  bool saveDiagramOutput();
  void writeCurrentDiagramOutputFile(rcstr filePath, rcstr separator, rcstr fileTag);
  void writeAllDataOutputFile(rcstr filePath, rcstr separator, rcstr fileTag);

private:
  qreal_vec xErrorAdd_, xErrorSub_, yErrorAdd_, yErrorSub_;
};

//------------------------------------------------------------------------------
}}
#endif  // OUTPUT_DIAGRAMS_H
