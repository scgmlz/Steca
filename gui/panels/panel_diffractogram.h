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

#ifndef PANEL_DIFFRACTOGRAM_H
#define PANEL_DIFFRACTOGRAM_H

#include "panel.h"
#include "actions.h"
#include "QCP/qcustomplot.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class DiffractogramPlot;

class DiffractogramPlotOverlay : public QWidget {
  CLASS(DiffractogramPlotOverlay) SUPER(QWidget)
public:
  DiffractogramPlotOverlay(DiffractogramPlot&);

  void setMargins(int left, int right);

private:
  DiffractogramPlot &plot_;

  QColor addColor_, remColor_, color_, bgColor_, reflColor_;
  int    marginLeft_, marginRight_;

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

class DiffractogramPlot : public QCustomPlot, protected RefHub {
  CLASS(DiffractogramPlot) SUPER(QCustomPlot)
public:
  enum class eTool {
    NONE,
    BACKGROUND,
    PEAK_REGION,
  };

  DiffractogramPlot(TheHub&, class Diffractogram&);

public:
  void  setTool(eTool);
  eTool getTool() const { return tool_; }

  void  plot(typ::Curve::rc, typ::Curve::rc, typ::Curve::rc,
             typ::curve_vec::rc, uint);

  typ::Range fromPixels(int, int);

  void clearBg();
  void addBg(typ::Range::rc);
  void remBg(typ::Range::rc);
  void setNewReflRange(typ::Range::rc);
  void updateBg();

  void clearReflLayer();

  QColor bgRgeColor_, reflRgeColor_;
  eFittingTab selectedFittingTab();

  void enterZoom(bool);

protected:
  void addBgItem(typ::Range::rc);
  void resizeEvent(QResizeEvent*);

private:
  Diffractogram &diffractogram_;

  eTool tool_;
  bool showBgFit_;

  QCPGraph *bgGraph_, *dgramGraph_, *dgramBgFittedGraph_, *dgramBgFittedGraph2_, *guesses_, *fits_;

  typ::vec<QCPGraph*>       reflGraph_;
  DiffractogramPlotOverlay *overlay_;
};

class Diffractogram : public PanelWidget {
  CLASS(Diffractogram) SUPER(PanelWidget)
public:
  Diffractogram(TheHub&);

  void render();

  data::shp_Dataset dataset() const { return dataset_; }

private:
  void setDataset(data::shp_Dataset);

  data::shp_Dataset dataset_;

  DiffractogramPlot *plot_;

  typ::Curve     dgram_, dgramBgFitted_, bg_;
  typ::curve_vec refls_;

  uint                 currReflIndex_;
  calc::shp_Reflection currentReflection_;

  QComboBox   *comboNormType_;
  QToolButton *enableZoom_;
  Action      *actZoom;

public:
  void calcDgram();
  void calcBackground();
  void calcReflections();

  void setCurrReflNewRange(typ::Range::rc);
  typ::Range currReflRange() const;
};

//------------------------------------------------------------------------------
}}
#endif // PANEL_DIFFRACTOGRAM_H
