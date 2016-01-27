#include "diffractogram.h"
#include <QPainter>
#include <QLineF>

namespace panel {

void Dgram::clear() {
  super::clear();
  maxInten = 0;
}

void Dgram::append(TthInt const& ti) {
  super::append(ti);
  maxInten = qMax(maxInten,ti.inten);
}

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
  xAxis->setLabel("2θ");
  yAxis->setLabel("I");
}

void DiffractogramPlot::plot(Dgram const& dgram) {
  clearGraphs();
  if (dgram.isEmpty()) {
    xAxis->setRange(0,1);
    yAxis->setRange(0,1);
  } else {
    xAxis->setRange(dgram.first().tth,dgram.last().tth);
    yAxis->setRange(0,dgram.maxInten);
    auto graph = addGraph();
  }
  replot();
}

void DiffractogramPlot::resizeEvent(QResizeEvent* e) {
  super::resizeEvent(e);
  auto size = e->size();
  overlay->setGeometry(0,0,size.width(),size.height());
}

//------------------------------------------------------------------------------

Diffractogram::Diffractogram(MainWin& mainWin,Session& session)
: super("Diffractogram",mainWin,session,Qt::Vertical), dataset(nullptr) {
  box->addWidget((plot = new DiffractogramPlot));
  box->addWidget(check("From all images"));

  connect(&session, &Session::datasetSelected, [this](pcCoreDataset dataset) {
    setDataset(dataset);
    // TODO trace - multiple unnecessary calls here?
  });
}

void Diffractogram::setDataset(pcCoreDataset dataset_) {
  dataset = dataset_;
  calcDgram();
  plot->plot(dgram);
}

void Diffractogram::refresh() {
  setDataset(dataset);
}

void Diffractogram::calcDgram() { // TODO is like getDgram00 w useCut==true, normalize==false
  dgram.clear();

  if (!dataset) return;

  // do this first! (e.g. before getCut())
  auto angles = session.calcAngleCorrArray(dataset->tthMitte());

  auto image    = dataset->getImage();
  auto imageCut = session.getImageCut();
  uint width    = imageCut.getWidth(image.getSize());
  uint pixTotal = imageCut.getCount(image.getSize());

  auto cut = session.getCut();
  qreal TTHMin = cut.tth_regular.low;
  qreal TTHMax = cut.tth_regular.hig;
  qreal deltaTTH = (TTHMax - TTHMin) / width;

  auto intens = image.getData();
  auto corr   = session.hasCorrFile() ? session.intensCorrArray.getData() : nullptr;

  // TODO bad! no iteration by floats
  ASSERT(deltaTTH>0) // TODO
  for (qreal tt = TTHMin + deltaTTH / 2; tt <= TTHMax - deltaTTH / 2; tt += deltaTTH) {
    TthInt ti;
    ti.inten = 0;
    ti.tth = tt;
    int countPixPerColumn = 0;

    for_i(pixTotal) { // TODO inefficient
      qreal tthPix = angles[i].tthPix;
      if ((tthPix > tt - deltaTTH / 2) && (tthPix <= tt + deltaTTH / 2)) {
        auto inten = intens[i];
        if (corr) inten *= corr[i];
        ti.inten += inten;
        countPixPerColumn++;
      }
    }

    if (countPixPerColumn > 0)
      ti.inten /= countPixPerColumn;

    dgram.append(ti);
  }
}

}

// eof
