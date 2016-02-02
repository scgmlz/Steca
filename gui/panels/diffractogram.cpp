#include "diffractogram.h"
#include <QPainter>
#include <QLineF>

namespace panel {

void Dgram::clear() {
  tth.clear(); inten.clear();
  maxInten = 0;
}

void Dgram::append(qreal tth_,qreal inten_) {
  tth.append(core::deg_rad(tth_)); inten.append(inten_);
  maxInten = qMax(maxInten,inten_);
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
    xAxis->setRange(dgram.tth.first(),dgram.tth.last());
    yAxis->setRange(0,dgram.maxInten);
    auto graph = addGraph();
    graph->setData(dgram.tth,dgram.inten);
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
  qreal TTHMin = cut.tth_regular.min;
  qreal TTHMax = cut.tth_regular.max;
  qreal deltaTTH = (TTHMax - TTHMin) / width;

  auto intens = image.getIntensities();
  auto corr   = session.hasCorrFile() ? session.intensCorrArray.getIntensities() : nullptr;

  // TODO bad! no iteration by floats
  ASSERT(deltaTTH>0) // TODO
  for (qreal tt = TTHMin + deltaTTH / 2; tt <= TTHMax - deltaTTH / 2; tt += deltaTTH) {
    qreal inten = 0;
    qreal tth = tt;
    int countPixPerColumn = 0;

    for_i(pixTotal) { // TODO inefficient
      qreal tthPix = angles[i].tthPix;
      if ((tthPix > tt - deltaTTH / 2) && (tthPix <= tt + deltaTTH / 2)) {
        auto in = intens[i];
        if (corr) in *= corr[i];
        inten += in;
        countPixPerColumn++;
      }
    }

    if (countPixPerColumn > 0)
      inten /= countPixPerColumn;

    dgram.append(tth,inten);
  }
}

}

// eof
