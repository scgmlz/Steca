#ifndef DIFFRACTOGRAM_H
#define DIFFRACTOGRAM_H

#include "panel.h"
#include "session.h"
#include "../3rd/qcustomplot.h"

namespace panel {

struct TthInt { // TODO rename 2 tthint_t
  qreal inten, tth;
};

struct Dgram: QVector<TthInt> { // TODO rename
  SUPER(Dgram,QVector<TthInt>)
  qreal maxInten;

  void clear();
  void append(TthInt const&);
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
  void setDataset(pcCoreDataset);
  void refresh();

  pcCoreDataset dataset;

  DiffractogramPlot *plot;

  Dgram dgram;
  void calcDgram();
};

}

#endif
