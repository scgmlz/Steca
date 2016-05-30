// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      output_polefigures.h
//! @brief     Output of pole data.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef OUTPUT_POLEFIGURES_H
#define OUTPUT_POLEFIGURES_H

#include "output_dialogs.h"

namespace gui { namespace output {
//------------------------------------------------------------------------------

class PoleFiguresParams : public Params {
  SUPER(PoleFiguresParams, Params)
public:
  using super::super;
};

//------------------------------------------------------------------------------

class TabGraph : public Tab {
  SUPER(TabGraph, Tab)
public:
  using deg = core::deg;
  using rad = core::rad;

  TabGraph(TheHub&, Params&);
  void set(core::ReflectionInfos);

protected:
  core::ReflectionInfos rs_;
  void paintEvent(QPaintEvent*);

  QPointF p(deg alpha, deg beta) const;
  void circle(QPointF c, qreal r);
  void paintGrid();
  void paintInfo();

  // valid during paintEvent
  QPainter *p_;
  QPointF   c_; qreal r_;

  bool  flat_;
  qreal alphaMax_ = 90;

  QCheckBox    *cbFlat_;
  QRadioButton *rb30_, *rb60_, *rb90_;
};

//------------------------------------------------------------------------------

class TabPoleFiguresSave : public TabSave {
  SUPER(TabPoleFiguresSave, TabSave)
public:
  TabPoleFiguresSave(TheHub& hub, Params& params);

  bool onlySelectedRefl() const;
  bool outputInten()      const;
  bool outputTth()        const;
  bool outputFWHM()       const;

protected:
  QRadioButton *rbSelectedRefl_,*rbAllRefls_;
  QCheckBox *outputInten_, *outputTth_, *outputFWHM_;
};

//------------------------------------------------------------------------------

class PoleFiguresFrame : public Frame {
  SUPER(PoleFiguresFrame, Frame)
public:
  PoleFiguresFrame(TheHub&, rcstr title, QWidget*);

protected:
  TabGraph *tabGraph_;
  TabPoleFiguresSave *tabSave_;

  void displayReflection(uint reflIndex, bool interpolated);

  bool savePoleFigureOutput();
  bool writePoleFigureOutputFiles(uint index);
  void writePoleFile(rcstr filePath,  core::ReflectionInfos, qreal_vec const&);
  void writeListFile(rcstr filePath,  core::ReflectionInfos, qreal_vec const&);
  void writeErrorMask(rcstr filePath, core::ReflectionInfos, qreal_vec const&);
};

//------------------------------------------------------------------------------

}}
#endif  // OUTPUT_POLEFIGURES_H
