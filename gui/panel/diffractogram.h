/** \file
 */

#ifndef DIFFRACTOGRAM_H
#define DIFFRACTOGRAM_H

#include "panel.h"
#include "core_dataset.h"
#include "core_types.h"
#include "approx/approx_methods.h"
#include "QCP/qcustomplot.h"

namespace panel {
//------------------------------------------------------------------------------

class TI_Curve: public core::Curve {
  SUPER(TI_Curve,core::Curve)
public:
  TI_Curve();

  void clear();
  void append(qreal tth,qreal inten);

  reals_t const& getTth()        const { return getXs(); }
  reals_t const& getInten()      const { return getYs(); }

  core::Range const& getTthRange()   const { return tthRange;   }
  core::Range const& getIntenRange() const { return intenRange; }

private:
  core::Range tthRange, intenRange;
};

class DiffractogramPlot;

class DiffractogramPlotOverlay: public QWidget {
  SUPER(DiffractogramPlotOverlay,QWidget)
public:
  DiffractogramPlotOverlay(DiffractogramPlot&);

  void setMargins(int left,int right);

private:
  DiffractogramPlot& plot;
  QColor addColor, remColor, color;
  int marginLeft, marginRight;

protected:
  void enterEvent(QEvent*);
  void leaveEvent(QEvent*);
  void mousePressEvent(QMouseEvent*);
  void mouseReleaseEvent(QMouseEvent*);
  void mouseMoveEvent(QMouseEvent*);

  void paintEvent(QPaintEvent*);

  bool hasCursor, mouseDown;
  int  cursorPos, mouseDownPos;

  void updateCursorRegion();
};

class Diffractogram;

class DiffractogramPlot: public QCustomPlot {
  SUPER(DiffractogramPlot,QCustomPlot)
public:
  enum Tool {
    TOOL_NONE,
    TOOL_BACKGROUND,
  };

  DiffractogramPlot(Diffractogram&);

  void setTool(Tool);
  Tool getTool() const { return tool; }

  void plotDgram(TI_Curve const&);
  void plotBg(TI_Curve const&);

  core::Range fromPixels(int,int);

  void clearBg();
  void addBg(core::Range const&);
  void remBg(core::Range const&);

protected:
  void resizeEvent(QResizeEvent*);

private:
  Diffractogram &diffractogram;
  Tool tool;
  QCPGraph *graph, *background;
  DiffractogramPlotOverlay *overlay;

  void updateBg();
};

class Diffractogram: public BoxPanel {
  SUPER(Diffractogram,BoxPanel)
  friend class DiffractogramPlot;
public:
  Diffractogram(MainWin&,Session&);

private:
  void setDataset(core::shp_Dataset);
  void refresh();

  core::shp_Dataset dataset;

  DiffractogramPlot *plot;

  TI_Curve dgram;
  TI_Curve bg;
  core::Ranges   bgRanges;
  core::approx::Polynomial bgPolynomial;

public:
  void calcDgram();
  void calcBg();
};

//------------------------------------------------------------------------------
}
#endif
