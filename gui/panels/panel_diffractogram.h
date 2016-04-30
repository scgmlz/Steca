// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      panel_diffractogram.h
//! @brief     File selection panel.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef PANEL_DIFFRACTOGRAM_H
#define PANEL_DIFFRACTOGRAM_H

#include "panel.h"
#include "core_dataset.h"
#include "core_reflection.h"
#include "core_fit_methods.h"
#include "types/core_type_curve.h"
#include "QCP/qcustomplot.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class DiffractogramPlot;

class DiffractogramPlotOverlay: public QWidget {
  SUPER(DiffractogramPlotOverlay,QWidget)
public:
  DiffractogramPlotOverlay(DiffractogramPlot&);

  void setMargins(int left,int right);

private:
  DiffractogramPlot& plot_;
  QColor addColor_, remColor_, color_, bgColor_, reflColor_;
  int marginLeft_, marginRight_;

protected:
  void enterEvent(QEvent*);
  void leaveEvent(QEvent*);
  void mousePressEvent(QMouseEvent*);
  void mouseReleaseEvent(QMouseEvent*);
  void mouseMoveEvent(QMouseEvent*);

  void paintEvent(QPaintEvent*);

  bool hasCursor_, mouseDown_;
  int  cursorPos_, mouseDownPos_;

  void updateCursorRegion();
};

class Diffractogram;

class DiffractogramPlot: public QCustomPlot, protected RefHub {
  SUPER(DiffractogramPlot,QCustomPlot)
public:
  enum Tool {
    TOOL_NONE,
    TOOL_BACKGROUND,
    TOOL_PEAK_REGION,
  };

  DiffractogramPlot(TheHub&,Diffractogram&);

public:
  void setTool(Tool);
  Tool getTool() const { return tool_; }

  void plot(core::rcCurve,core::rcCurve, core::rcCurve, core::curve_vec const&, uint);

  core::Range fromPixels(int,int);

  void clearBg();
  void addBg(core::rcRange);
  void remBg(core::rcRange);
  void setNewReflRange(core::rcRange);
  void updateBg();

  void clearReflLayer();

  QColor rangeColor_;
  int selectedFittingTab();

protected:
  void addBgItem(core::rcRange);
  void resizeEvent(QResizeEvent*);

private:
  Diffractogram &diffractogram_;
  Tool tool_;
  QCPGraph *bgGraph_, *dgramGraph_, *dgramBgFittedGraph_, *guesses_, *fits_;
  QVector<QCPGraph*> reflGraph_;
  DiffractogramPlotOverlay *overlay_;
  bool showBgFit_;
};

class Diffractogram: public BoxPanel {
  SUPER(Diffractogram,BoxPanel)
public:
  Diffractogram(TheHub&);

  core::shp_Dataset const& getDataset() const { return dataset_; }
  void render(); // TODO move to DiffractogramPlot (?)

  QCheckBox *avgCurveCheckBox, *fixedIntenCheckBox;

private:
  void setDataset(core::shp_Dataset);

  core::shp_Dataset dataset_;

  DiffractogramPlot *plot_;

  core::Curve  dgram_, dgramBgFitted_, bg_;
  core::curve_vec refls_;

  uint currReflIndex_;
  core::shp_Reflection currentReflection_;

public:
  void calcDgram();
  void calcBackground();
  void calcReflections();

  void setCurrReflNewRange(core::rcRange);
  core::Range currReflRange() const;
};

//------------------------------------------------------------------------------
  }}
#endif // PANEL_DIFFRACTOGRAM_H
