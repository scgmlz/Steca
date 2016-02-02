#ifndef DIFFRACTOGRAM_H
#define DIFFRACTOGRAM_H

#include "panel.h"
#include "session.h"
#include "../3rd/qcustomplot.h"

namespace panel {

struct Dgram { // TODO rename
  QVector<qreal> tth,inten; qreal maxInten;
  void clear();
  bool isEmpty() const { return tth.isEmpty(); }
  void append(qreal tth,qreal inten);
};

class DiffractogramPlot;

class DiffractogramPlotOverlay: public QWidget {
  SUPER(DiffractogramPlotOverlay,QWidget)
public:
  DiffractogramPlotOverlay(DiffractogramPlot&);

protected:
  void enterEvent(QEvent*);
  void leaveEvent(QEvent*);
  void mouseMoveEvent(QMouseEvent*);

  void paintEvent(QPaintEvent*);

  bool  hasCursor;
  int   cursorPos;

  void updateCursorRegion();
};

class DiffractogramPlot: public QCustomPlot {
  SUPER(DiffractogramPlot,QCustomPlot)
public:
  DiffractogramPlot();

  void plot(Dgram const&);

protected:
  void resizeEvent(QResizeEvent*);

private:
  DiffractogramPlotOverlay *overlay;
};

class Diffractogram: public BoxPanel {
  SUPER(Diffractogram,BoxPanel)
public:
  Diffractogram(MainWin&,Session&);

private:
  void setDataset(core::shp_Dataset);
  void refresh();

  core::shp_Dataset dataset;

  DiffractogramPlot *plot;

  Dgram dgram;
  void calcDgram();
};

}

#endif
