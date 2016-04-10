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
  DiffractogramPlot& _plot;
  QColor _addColor, _remColor, _color;
  int _marginLeft, _marginRight;

protected:
  void enterEvent(QEvent*);
  void leaveEvent(QEvent*);
  void mousePressEvent(QMouseEvent*);
  void mouseReleaseEvent(QMouseEvent*);
  void mouseMoveEvent(QMouseEvent*);

  void paintEvent(QPaintEvent*);

  bool _hasCursor, _mouseDown;
  int  _cursorPos, _mouseDownPos;

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
  Tool getTool() const { return _tool; }

  void plot(core::rcCurve,core::rcCurve, core::rcCurve, core::curve_vec const&, uint);

  core::Range fromPixels(int,int);

  void clearBg();
  void addBg(core::rcRange);
  void remBg(core::rcRange);
  void setNewReflRange(core::rcRange);
  void updateBg();

  void clearReflLayer();

protected:
  void addBgItem(core::rcRange);
  void resizeEvent(QResizeEvent*);

private:
  Diffractogram &_diffractogram;
  Tool _tool;
  QCPGraph *_bgGraph, *_dgramGraph, *_dgramBgFittedGraph, *_guesses, *_fits;
  QVector<QCPGraph*> _reflGraph;
  DiffractogramPlotOverlay *_overlay;
  bool _showBgFit;
};

class Diffractogram: public BoxPanel {
  SUPER(Diffractogram,BoxPanel)
public:
  Diffractogram(TheHub&);

  core::shp_Dataset const& getDataset() const { return _dataset; }
  void renderDataset(); // TODO move to DiffractogramPlot (?)

private:
  void setDataset(core::shp_Dataset);

  core::shp_Dataset _dataset;

  DiffractogramPlot *_plot;

  core::Curve  _dgram, _dgramBgFitted, _bg;
  core::curve_vec _refls;

  uint _currReflIndex;
  core::shp_Reflection _currentReflection;

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
