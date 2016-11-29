/*******************************************************************************
 * STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

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
