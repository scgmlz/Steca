#include "diffractogram.h"
#include "thehub.h"
#include "core_fit_fitting.h"
#include "core_types.h"

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
  case DiffractogramPlot::TOOL_BACKGROUND:
    if (Qt::LeftButton == e->button())
      plot.addBg(range);
    else
      plot.remBg(range);
    break;

  case DiffractogramPlot::TOOL_PEAK_REGION:
    plot.setNewReflRange(range);
    break;

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
  // updating 2 pixels seems to work both on Linux & Mac
  update(cursorPos-1,g.top(),2,g.height());
}

DiffractogramPlot::DiffractogramPlot(TheHub& theHub_,Diffractogram& diffractogram_)
: theHub(theHub_), diffractogram(diffractogram_) {
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

  // graphs in the "main" layer; in the display order
  bgGraph             = addGraph();
  dgramGraph          = addGraph();
  dgramBgFittedGraph  = addGraph();

  bgGraph->setPen(QPen(Qt::green,0));
  dgramGraph->setPen(QPen(Qt::gray));
  dgramBgFittedGraph->setPen(QPen(Qt::blue,2));

  // background regions
  addLayer("bg",layer("background"),QCustomPlot::limAbove);
  // reflections
  addLayer("refl",layer("main"),QCustomPlot::limAbove);
  // reflections
  addLayer("marks",layer("refl"),QCustomPlot::limAbove);

  setCurrentLayer("marks");

  guesses = addGraph();
  guesses->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 8));
  guesses->setLineStyle(QCPGraph::lsNone);
  guesses->setPen(QPen(Qt::darkGray));

  fits = addGraph();
  fits->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 8));
  fits->setLineStyle(QCPGraph::lsNone);
  fits->setPen(QPen(Qt::red));

  connect(&theHub, &TheHub::reflectionData, [this](core::shp_Reflection reflection) {
    guesses->clearData();
    fits->clearData();
    if (reflection) {
      auto &fun = reflection->getPeakFunction();

      auto gp = fun.getGuessPeak();
      guesses->addData(gp.x,gp.y);
      auto gw2 = fun.getGuessFWHM() / 2;
      guesses->addData(gp.x-gw2,gp.y/2);
      guesses->addData(gp.x+gw2,gp.y/2);

      auto fp = fun.getFitPeak();
      fits->addData(fp.x,fp.y);
      auto fw2 = fun.getFitFWHM() / 2;
      fits->addData(fp.x-fw2,fp.y/2);
      fits->addData(fp.x+fw2,fp.y/2);
    }
  });

  tool = TOOL_NONE;
}

void DiffractogramPlot::setTool(Tool tool_) {
  tool = tool_;
  updateBg();
}

void DiffractogramPlot::plot(
  core::Curve const& dgram, core::Curve const& dgramBgFitted, core::Curve const& bg,
  core::Curves const& refls, uint currReflIndex
) {
  if (dgram.isEmpty()) {
    xAxis->setVisible(false);
    yAxis->setVisible(false);

    bgGraph->clearData();
    dgramGraph->clearData();
    dgramBgFittedGraph->clearData();

    clearReflLayer();

  } else {
    auto tthRange   = dgram.getXRange();
    bool fixedIntensityScale = theHub.fixedIntensityScale;

    core::Range intenRange;
    if (fixedIntensityScale) {
      auto max = diffractogram.dataset->getRgeIntens(true).max;
      // heuristics; to calculate this precisely would require much more computation
      intenRange = core::Range(-max/30,max/3);
    } else {
      intenRange = dgramBgFitted.getYRange();
      intenRange.extend(dgram.getYRange());
    }

    xAxis->setRange(tthRange.min,tthRange.max);
    yAxis->setRange(qMin(0.,intenRange.min),intenRange.max);
    xAxis->setVisible(true);
    yAxis->setVisible(true);

    if (diffractogram.showBgFit) {
      bgGraph->setData(bg.getXs(), bg.getYs());
    } else {
      bgGraph->clearData();
    }

    dgramGraph->setData(dgram.getXs(), dgram.getYs());
    dgramBgFittedGraph->setData(dgramBgFitted.getXs(), dgramBgFitted.getYs());

    clearReflLayer();
    setCurrentLayer("refl");

    for_i (refls.count()) {
      auto &r = refls[i];
      auto *graph = addGraph(); reflGraph.append(graph);
      graph->setPen(QPen(Qt::green,i==(int)currReflIndex ? 2 : 1));
      graph->setData(r.getXs(), r.getYs());
    }
  }

  replot();
}

core::Range DiffractogramPlot::fromPixels(int pix1, int pix2) {
  return core::Range::safeFrom(
    xAxis->pixelToCoord(pix1),
    xAxis->pixelToCoord(pix2));
}

void DiffractogramPlot::clearBg() {
  theHub.getBgRanges().clear();
  updateBg();
}

void DiffractogramPlot::addBg(core::Range const& range) {
  if (theHub.getBgRanges().add(range)) updateBg();
}

void DiffractogramPlot::remBg(core::Range const& range) {
  if (theHub.getBgRanges().rem(range)) updateBg();
}

void DiffractogramPlot::setNewReflRange(core::Range const& range) {
  diffractogram.setCurrReflNewRange(range);
  updateBg();
}

void DiffractogramPlot::updateBg() {
  clearItems();

  switch (tool) {
  default:
    break;
  case TOOL_BACKGROUND: {
    core::Ranges const& rs = theHub.getBgRanges();
    for_i (rs.count()) addBgItem(rs.at(i));
    break;
  }
  case TOOL_PEAK_REGION:
    addBgItem(diffractogram.currReflRange());
    break;
  }

  diffractogram.renderDataset();
}

void DiffractogramPlot::clearReflLayer() {
  for (auto g: reflGraph) removeGraph(g);
  reflGraph.clear();
}

void DiffractogramPlot::addBgItem(core::Range const& range) {
  setCurrentLayer("bg");
  auto bgColor = QColor(0xff,0xf0,0xf0);

  auto ir = new QCPItemRect(this);
  ir->setPen(QPen(bgColor));
  ir->setBrush(QBrush(bgColor));
  auto br = ir->bottomRight;
  br->setTypeY(QCPItemPosition::ptViewportRatio);
  br->setCoords(range.max,1);
  auto tl = ir->topLeft;
  tl->setTypeY(QCPItemPosition::ptViewportRatio);
  tl->setCoords(range.min,0);
  addItem(ir);
}

void DiffractogramPlot::resizeEvent(QResizeEvent* e) {
  super::resizeEvent(e);
  auto size = e->size();
  overlay->setGeometry(0,0,size.width(),size.height());
}

//------------------------------------------------------------------------------

Diffractogram::Diffractogram(TheHub& theHub_)
: super(EMPTY_STR,theHub_,Qt::Vertical), dataset(nullptr)
, showBgFit(false), currReflIndex(-1) {
  box->addWidget((plot = new DiffractogramPlot(theHub,*this)));

  connect(&theHub, &TheHub::datasetSelected, [this](core::shp_Dataset dataset) {
    setDataset(dataset);
  });

  connect(&theHub, &TheHub::geometryChanged, [this]() {
    renderDataset();
  });

  connect(&theHub, &TheHub::correctionEnabled, [this]() {
    renderDataset();
  });

  connect(&theHub, &TheHub::displayChange, [this]() {
    renderDataset();
  });

  connect(&theHub, &TheHub::backgroundPolynomialDegree, [this](uint degree) {
    theHub.getBgPolynomialDegree() = degree; // keep session up-to-date
    renderDataset();
  });

  connect(&theHub, &TheHub::normChanged, [this]() {
    renderDataset();
  });

  // REVIEW all these connects
  connect(theHub.actBackgroundClear, &QAction::triggered, [this]() {
    plot->clearBg();
  });

  connect(theHub.actBackgroundBackground, &QAction::toggled, [this](bool on) {
    if (on) theHub.actReflectionRegion->setChecked(false); // works as radio
    plot->setTool(on ? DiffractogramPlot::TOOL_BACKGROUND : DiffractogramPlot::TOOL_NONE);
  });

  connect(theHub.actBackgroundShowFit, &QAction::toggled, [this](bool on) {
    showBgFit = on;
    plot->updateBg();
  });

  connect(theHub.actReflectionRegion, &QAction::toggled, [this](bool on) {
    if (on) theHub.actBackgroundBackground->setChecked(false); // works as radio
    plot->setTool(on ? DiffractogramPlot::TOOL_PEAK_REGION : DiffractogramPlot::TOOL_NONE);
  });

  connect(&theHub, &TheHub::reflectionSelected, [this](core::shp_Reflection reflection) {
    currentReflection = reflection;
    plot->updateBg();
  });

  connect(&theHub, &TheHub::reflectionValues, [this](core::Range const& range, core::XY const& peak, qreal fwhm, bool withGuesses) {
    if (currentReflection) {
      currentReflection->setRange(range);
      if (withGuesses)
        currentReflection->invalidateGuesses();
      else {
        currentReflection->setGuessPeak(peak);
        currentReflection->setGuessFWHM(fwhm);
      }
      plot->updateBg();
    }
  });

  theHub.actBackgroundShowFit->setChecked(true);
}

void Diffractogram::setDataset(core::shp_Dataset dataset_) {
  dataset = dataset_;
  renderDataset();
}

void Diffractogram::renderDataset() {
  calcDgram();
  calcBackground();
  calcReflections();

  plot->plot(dgram,dgramBgFitted,bg,refls,currReflIndex);
}

void Diffractogram::calcDgram() { // TODO is like getDgram00 w useCut==true, normalize==false
  dgram.clear();

  if (!dataset) return;

  const auto cut = theHub.getCut();
  dgram = makeCurve(theHub.allLenses(*dataset),
                    cut.gamma, cut.tth_regular);
}

void Diffractogram::calcBackground() {
  bg.clear(); dgramBgFitted.clear();

  auto bgPolynomialDegree = theHub.getBgPolynomialDegree();  // not very nice REVIEW
  auto &bgRanges     = theHub.getBgRanges();      // not very nice REVIEW

  auto bgPolynomial = core::fit::fitBackground(dgram,bgRanges,bgPolynomialDegree);
  auto& tth   = dgram.getXs();
  auto& inten = dgram.getYs();
  for_i (dgram.count()) {
    qreal x = tth[i], y = bgPolynomial.y(x);
    bg.append(x,y);
    dgramBgFitted.append(x,inten[i] - y);
  }
}

void Diffractogram::setCurrReflNewRange(core::Range const& range) {
  if (currentReflection) {
    currentReflection->setRange(range);
    currentReflection->invalidateGuesses();
  }
}

core::Range Diffractogram::currReflRange() const {
  return currentReflection ? currentReflection->getRange() : core::Range();
}

void Diffractogram::calcReflections() {
  refls.clear(); currReflIndex = -1;

  auto rs = theHub.getReflections();
  for_i (rs.count()) {
    auto &r = rs[i];
    if (r == currentReflection)
      currReflIndex = i;

    auto range = r->getRange();
    if (range.min < range.max) {
      auto &fun = r->getPeakFunction();
      core::fit::fitPeak(fun,dgramBgFitted,range);

      auto &tth   = dgramBgFitted.getXs();

      core::Curve c;

      for_i (dgramBgFitted.count()) {
        qreal x = tth[i];
        if (range.contains(x)) {
          c.append(x,fun.y(x));
        }
      }

      refls.append(c);
    }
  }

  theHub.setReflectionData(currentReflection);
}

//------------------------------------------------------------------------------
}
// eof
