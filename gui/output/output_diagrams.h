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
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef OUTPUT_DIAGRAMS_H
#define OUTPUT_DIAGRAMS_H

#include "output_dialogs.h"
#include "QCP/qcustomplot.h"

namespace gui { namespace output {
//------------------------------------------------------------------------------

class DiagramsParams : public Params {
  CLS(DiagramsParams) SUPER(Params)
public:
  DiagramsParams(TheHub&);

protected:
  panel::GridPanel *gpAxes_;
public:
  QComboBox *xAxis, *yAxis;
};

//------------------------------------------------------------------------------

class TabPlot : public QCustomPlot {
  CLS(TabPlot) SUPER(QCustomPlot)
public:
  TabPlot();
  void set(calc::ReflectionInfos);

  void plot(qreal_vec::rc xs,    qreal_vec::rc ys,
            qreal_vec::rc ysAdd, qreal_vec::rc ysSub);

protected:
  QCPGraph *graph_, *graphAdd_, *graphSub_;
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

  DiagramsParams const* params() const {
    return static_cast<DiagramsParams*>(params_);
  }

  eReflAttr xAttr() const;
  eReflAttr yAttr() const;

  void displayReflection(int reflIndex, bool interpolated);

  calc::ReflectionInfos rs_;
  qreal_vec xs_, ys_, ysErrorAdd_, ysErrorSub_;

  void recalculate();

  bool saveDiagramOutput();
  void writeCurrentDiagramOutputFile(rcstr filePath, rcstr separator, rcstr fileTag);
  void writeAllDataOutputFile(rcstr filePath, rcstr separator, rcstr fileTag);
};

//------------------------------------------------------------------------------
}}
#endif // OUTPUT_DIAGRAMS_H
