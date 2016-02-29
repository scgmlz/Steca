#include "diffractogram.h"
#include "mainwin.h"
#include "session.h"
#include "core_fit_fitting.h"

namespace panel {
//------------------------------------------------------------------------------

DiffractogramPlotOverlay::DiffractogramPlotOverlay(DiffractogramPlot& plot_)
: super(&plot_), plot(plot_), hasCursor(false), mouseDown(false), cursorPos(0)
, mouseDownPos(0) {

  setMouseTracking(true);
  setMargins(0,0);

  addColor = QColor(0xff,0xf0,0xf0,0x80);
  remColor = QColor(0xf0,0xf0,0xff,0x80);
}

void DiffractogramPlotOverlay::setMargins(int left, int right) {
  marginLeft = left; marginRight = right;
}

void DiffractogramPlotOverlay::enterEvent(QEvent*) {
  hasCursor = true;
  updateCursorRegion();
}

void DiffractogramPlotOverlay::leaveEvent(QEvent*) {
  hasCursor = false;
  updateCursorRegion();
}

void DiffractogramPlotOverlay::mousePressEvent(QMouseEvent* e) {
  mouseDownPos = cursorPos;
  mouseDown = true;
  color = Qt::LeftButton == e->button() ? addColor : remColor;
  update();
}

void DiffractogramPlotOverlay::mouseReleaseEvent(QMouseEvent* e) {
  mouseDown = false;
  update();

  core::Range range(plot.fromPixels(mouseDownPos,cursorPos));
  switch (plot.getTool()) {
  case DiffractogramPlot::TOOL_BACKGROUND: {
    if (Qt::LeftButton == e->button())
      plot.addBg(range);
    else
      plot.remBg(range);
  }
  default:
    break;
  }
}

void DiffractogramPlotOverlay::mouseMoveEvent(QMouseEvent* e) {
  updateCursorRegion();
  cursorPos = qBound(marginLeft,e->x(),width()-marginRight);
  updateCursorRegion();
  if (mouseDown) update();
}

void DiffractogramPlotOverlay::paintEvent(QPaintEvent*) {
  QPainter painter(this);
  QRect g = geometry();

  if (mouseDown) {
    g.setLeft(qMin(mouseDownPos,cursorPos));
    g.setRight(qMax(mouseDownPos,cursorPos));

    painter.fillRect(g,color);
  }

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

DiffractogramPlot::DiffractogramPlot(Diffractogram& diffractogram_): diffractogram(diffractogram_) {
  overlay = new DiffractogramPlotOverlay(*this);

  auto *ar = axisRect();

  // fix margins
  QFontMetrics fontMetrics(font());
  int em = fontMetrics.width('M'), ascent = fontMetrics.ascent();

  QMargins margins(3*em,ascent,em,2*ascent);
  ar->setAutoMargins(QCP::msNone);
  ar->setMargins(margins);
  overlay->setMargins(margins.left(),margins.right());

  // colours
  setBackground(palette().background().color());
  ar->setBackground(Qt::white);

  // one graph in the "main" layer
  graph = addGraph();
  background = addGraph();

  // background regions
  addLayer("bg",layer("background"),QCustomPlot::limAbove);

  setTool(TOOL_NONE);
}

void DiffractogramPlot::setTool(Tool tool_) {
  tool = tool_;
}

void DiffractogramPlot::plotDgram(core::TI_Curve const& dgram) {
  if (dgram.isEmpty()) {
    xAxis->setVisible(false);
    yAxis->setVisible(false);
    graph->clearData();
  } else {
    auto tthRange   = dgram.getTthRange();
    bool globalNorm = diffractogram.session.isGlobalNorm();
    auto intenRange = globalNorm
                        // TODO to calculate this global range precisely
                        // would require all diagram recomputation
                        ? diffractogram.dataset->getRgeIntens(true)
                        : dgram.getIntenRange();
    if (globalNorm) intenRange = core::Range(intenRange.min,intenRange.max / 3);
    xAxis->setRange(tthRange.min,tthRange.max);
    yAxis->setRange(qMin(0.,intenRange.min),intenRange.max);
    xAxis->setVisible(true);
    yAxis->setVisible(true);

    graph->setData(dgram.getTth(),dgram.getInten());
  }

  replot();
}

void DiffractogramPlot::plotBg(core::TI_Curve const& bg) {
  if (bg.isEmpty()) {
    background->clearData();
  } else {
    background->setData(bg.getTth(),bg.getInten());
  }

  replot();
}

core::Range DiffractogramPlot::fromPixels(int pix1, int pix2) {
  return core::Range::safeFrom(
    xAxis->pixelToCoord(pix1),
    xAxis->pixelToCoord(pix2));
}

void DiffractogramPlot::clearBg() {
  diffractogram.bgRanges.clear();
  updateBg();
}

void DiffractogramPlot::addBg(core::Range const& range) {
  if (diffractogram.bgRanges.add(range)) updateBg();
}

void DiffractogramPlot::remBg(core::Range const& range) {
  if (diffractogram.bgRanges.rem(range)) updateBg();
}

void DiffractogramPlot::resizeEvent(QResizeEvent* e) {
  super::resizeEvent(e);
  auto size = e->size();
  overlay->setGeometry(0,0,size.width(),size.height());
}

void DiffractogramPlot::updateBg() {
  clearItems();

  setCurrentLayer("bg");

  auto bgColor = QColor(0xff,0xf8,0xf8);
  auto const& rs = diffractogram.bgRanges;
  for_i (rs.count()) {
    auto const& r = rs.at(i);
    auto ir = new QCPItemRect(this);
    ir->setPen(QPen(bgColor));
    ir->setBrush(QBrush(bgColor));
    auto br = ir->bottomRight;
    br->setTypeY(QCPItemPosition::ptViewportRatio);
    br->setCoords(r.max,1);
    auto tl = ir->topLeft;
    tl->setTypeY(QCPItemPosition::ptViewportRatio);
    tl->setCoords(r.min,0);
    addItem(ir);
  }

  diffractogram.fitBackground();
  plotBg(diffractogram.bg);
}

//------------------------------------------------------------------------------

Diffractogram::Diffractogram(MainWin& mainWin,Session& session)
: super(EMPTY_STR,mainWin,session,Qt::Vertical), dataset(nullptr), bgPolynomialDegree(0) {
  box->addWidget((plot = new DiffractogramPlot(*this)));

  connect(&session, &Session::datasetSelected, [this](core::shp_Dataset dataset) {
    setDataset(dataset);
  });

  connect(&session, &Session::geometryChanged, [this]() {
    renderDataset();
  });

  connect(&session, &Session::correctionEnabled, [this]() {
    renderDataset();
  });

  connect(&session, &Session::displayChange, [this]() {
    renderDataset();
  });

  connect(&session, &Session::backgroundPolynomialDegree, [this](uint degree) {
    bgPolynomialDegree = degree;
    renderDataset();
  });

  connect(mainWin.actBackgroundBackground, &QAction::toggled, [this](bool on) {
    if (on) plot->clearBg();
    plot->setTool(on ? DiffractogramPlot::TOOL_BACKGROUND : DiffractogramPlot::TOOL_NONE);
  });
}

void Diffractogram::setDataset(core::shp_Dataset dataset_) {
  dataset = dataset_;
  renderDataset();
}

void Diffractogram::renderDataset() {
  calcDgram();
  plot->plotDgram(dgram);
  fitBackground();
  plot->plotBg(bg);
}

void Diffractogram::calcDgram() { // TODO is like getDgram00 w useCut==true, normalize==false
  dgram.clear();

  if (!dataset) return;

  // do this first! (e.g. before getCut())
  auto angles   = session.calcAngleCorrArray(dataset->tthMitte());

  auto image    = dataset->getImage();
  auto imageCut = session.getImageCut();
  uint width    = imageCut.getWidth(session.getImageSize());
  uint height   = imageCut.getHeight(session.getImageSize());

  auto cut = session.getCut();
  qreal TTHMin = cut.tth_regular.min;
  qreal TTHMax = cut.tth_regular.max;
  qreal deltaTTH = (TTHMax - TTHMin) / width;

  reals_t intens_vec(width);
  uints_t counts_vec(width,0);

  for_i (height) {
    auto iy = i + imageCut.top;
    for_i (width) {
      auto ix = i + imageCut.left;

      // TODO angles can be arranged for a single loop for_i (pixTotal) [last in commit 98413db71cd38ebaa54b6337a6c6e670483912ef]
      auto tthPix = angles.at(session.pixIndexNoTransform(ix,iy)).tthPix;

      int bin = (tthPix==TTHMax) ? width-1 : qFloor((tthPix - TTHMin) / deltaTTH);
      if (bin<0 || (int)width<=bin) continue;  // outside of the cut

      auto in = session.pixIntensity(image,ix,iy);
      if (!qIsNaN(in)) {
        intens_vec[bin] += in;
        counts_vec[bin]++;
      }
    }
  }

  for_i (width) {
    auto in = intens_vec[i]; auto cnt = counts_vec[i];
    if (cnt > 0) in /= cnt;
    dgram.append(core::deg_rad(TTHMin + deltaTTH*i),in);
  }
}

void Diffractogram::fitBackground() {
  bg = core::fit::fitBackground(dgram,bgRanges,bgPolynomialDegree);
}

//------------------------------------------------------------------------------
}
// eof
