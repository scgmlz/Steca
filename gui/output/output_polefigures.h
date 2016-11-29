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

#ifndef OUTPUT_POLEFIGURES_H
#define OUTPUT_POLEFIGURES_H

#include "output_dialogs.h"

namespace gui { namespace output {
//------------------------------------------------------------------------------

class TabGraph : public Tab {
  CLS(TabGraph) SUPER(Tab)
public:
  using deg = typ::deg;
  using rad = typ::rad;

  TabGraph(TheHub&, Params&);
  void set(calc::ReflectionInfos);

protected:
  void update();

  calc::ReflectionInfos rs_;
  void paintEvent(QPaintEvent*);

  QPointF p(deg alpha, deg beta) const;
  deg alpha(QPointF const&)      const;
  deg beta(QPointF  const&)      const;

  void circle(QPointF c, qreal r);

  void paintGrid();
  void paintPoints();

  // valid during paintEvent
  QPainter *p_;
  QPointF   c_; qreal r_;

  bool  flat_;
  qreal alphaMax_, avgAlphaMax_;

  QCheckBox    *cbFlat_;
};

//------------------------------------------------------------------------------

class TabPoleFiguresSave : public TabSave {
  CLS(TabPoleFiguresSave) SUPER(TabSave)
public:
  TabPoleFiguresSave(TheHub& hub, Params& params);

  bool onlySelectedRefl() const;
  bool outputInten()      const;
  bool outputTth()        const;
  bool outputFWHM()       const;

  void rawReflSettings(bool on);

protected:
  QRadioButton *rbSelectedRefl_,*rbAllRefls_;
  QCheckBox *outputInten_, *outputTth_, *outputFWHM_;
};

//------------------------------------------------------------------------------

class PoleFiguresFrame : public Frame {
  CLS(PoleFiguresFrame) SUPER(Frame)
public:
  PoleFiguresFrame(TheHub&, rcstr title, QWidget*);

protected:
  TabGraph           *tabGraph_;
  TabPoleFiguresSave *tabSave_;

  void displayReflection(uint reflIndex, bool interpolated);

  bool savePoleFigureOutput();
  bool writePoleFigureOutputFiles(rcstr filePath, uint index);
  void writePoleFile(rcstr filePath,  calc::ReflectionInfos, qreal_vec::rc);
  void writeListFile(rcstr filePath,  calc::ReflectionInfos, qreal_vec::rc);
  void writeErrorMask(rcstr filePath, calc::ReflectionInfos, qreal_vec::rc);
};

//------------------------------------------------------------------------------
}}
#endif // OUTPUT_POLEFIGURES_H
