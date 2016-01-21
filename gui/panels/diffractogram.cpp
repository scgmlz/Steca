#include "diffractogram.h"
#include <QPainter>
#include <QLineF>

namespace panel {

DiffractogramPlotOverlay::DiffractogramPlotOverlay(DiffractogramPlot& plot)
  : super(&plot), hasCursor(false), cursorPos(0) {
  setMouseTracking(true);
}

void DiffractogramPlotOverlay::enterEvent(QEvent*) {
  hasCursor = true;
  updateCursorRegion();
}

void DiffractogramPlotOverlay::leaveEvent(QEvent*) {
  hasCursor = false;
  updateCursorRegion();
}

void DiffractogramPlotOverlay::mouseMoveEvent(QMouseEvent* e) {
  updateCursorRegion();
  cursorPos = e->x();
  updateCursorRegion();
}

void DiffractogramPlotOverlay::paintEvent(QPaintEvent*) {
  QPainter painter(this);
  auto g = geometry();

  if (hasCursor) {
    QLineF cursor(cursorPos,g.top(),cursorPos,g.bottom());

    painter.setPen(Qt::red);
    painter.drawLine(cursor);
  }
}

void DiffractogramPlotOverlay::updateCursorRegion() {
  auto g = geometry();
  update(cursorPos,g.top(),1,g.height());
}

DiffractogramPlot::DiffractogramPlot() {
  overlay = new DiffractogramPlotOverlay(*this);
}

void DiffractogramPlot::resizeEvent(QResizeEvent* e) {
  super::resizeEvent(e);
  auto size = e->size();
  overlay->setGeometry(0,0,size.width(),size.height());
}

Diffractogram::Diffractogram(MainWin& mainWin,Session& session)
: super("Diffractogram",mainWin,session,Qt::Vertical) {
  box->addWidget(new DiffractogramPlot);
  box->addWidget(check("From all images"));
}

}

// eof
