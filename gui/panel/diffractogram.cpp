#include "diffractogram.h"
#include "mainwin.h"
#include "session.h"

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

  // allocate margins
  QFontMetrics fontMetrics(font());
  int em = fontMetrics.width('M'), ascent = fontMetrics.ascent();

  auto *r = axisRect();
  r->setAutoMargins(QCP::msNone);
  r->setMargins(QMargins(3*em,ascent,em,2*ascent));

  // one graph
  graph = addGraph();
}

void DiffractogramPlot::plot(Dgram const& dgram) {
  if (dgram.isEmpty()) {
    xAxis->setVisible(false);
    yAxis->setVisible(false);

    graph->clearData();
  } else {
    xAxis->setRange(dgram.tth.first(),dgram.tth.last());
    yAxis->setRange(0,dgram.maxInten);
    xAxis->setVisible(true);
    yAxis->setVisible(true);

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

  connect(&session, &Session::datasetSelected, [this](core::shp_Dataset dataset) {
    setDataset(dataset);
  });

  connect(&session, &Session::geometryChanged, [this]() {
    this->refresh();
  });
}

void Diffractogram::setDataset(core::shp_Dataset dataset_) {
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
  int  width    = imageCut.getWidth(image.getSize().width());
  uint pixTotal = imageCut.getCount(image.getSize().height());

  auto cut = session.getCut();
  qreal TTHMin = cut.tth_regular.min;
  qreal TTHMax = cut.tth_regular.max;
  qreal deltaTTH = (TTHMax - TTHMin) / width;

  auto intens = image.getIntensities();
  auto corr   = session.hasCorrFile() ? session.intensCorrArray.getIntensities() : nullptr;

  QVector<qreal> intens_vec(width);
  QVector<uint>  counts_vec(width,0);

  for_i(pixTotal) {
    auto tthPix = angles[i].tthPix;
    int bin = (tthPix==TTHMax) ? width-1 : qFloor((tthPix - TTHMin) / deltaTTH);

    if(bin<0 || width<=bin) continue;  // outside of the cut

    auto in = intens[i];
    if (corr) {
      auto factor = corr[i];
      if (qIsNaN(factor)) continue;  // skip these pixels
      in *= factor;
    }
    intens_vec[bin] += in;
    counts_vec[bin]++;
  }

  for_i(width) {
    auto in = intens_vec[i]; auto cnt = counts_vec[i];
    if (cnt > 0) in /= cnt;
    dgram.append(TTHMin + deltaTTH*i,in);
  }
}

}

// eof
