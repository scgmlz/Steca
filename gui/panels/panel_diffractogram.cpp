// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      panel_diffractogram.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "panel_diffractogram.h"
#include "thehub.h"
#include "core_fit_fitting.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

DiffractogramPlotOverlay::DiffractogramPlotOverlay(DiffractogramPlot& plot_)
: super(&plot_), _plot(plot_), _hasCursor(false), _mouseDown(false), _cursorPos(0)
, _mouseDownPos(0) {

  setMouseTracking(true);
  setMargins(0,0);

  _addColor = QColor(0xff,0xf0,0xf0,0x80);
  _remColor = QColor(0xf0,0xf0,0xff,0x80);
}

void DiffractogramPlotOverlay::setMargins(int left, int right) {
  _marginLeft = left; _marginRight = right;
}

void DiffractogramPlotOverlay::enterEvent(QEvent*) {
  _hasCursor = true;
  updateCursorRegion();
}

void DiffractogramPlotOverlay::leaveEvent(QEvent*) {
  _hasCursor = false;
  updateCursorRegion();
}

void DiffractogramPlotOverlay::mousePressEvent(QMouseEvent* e) {
  _mouseDownPos = _cursorPos;
  _mouseDown = true;
  _color = Qt::LeftButton == e->button() ? _addColor : _remColor;
  update();
}

void DiffractogramPlotOverlay::mouseReleaseEvent(QMouseEvent* e) {
  _mouseDown = false;
  update();

  core::Range range(_plot.fromPixels(_mouseDownPos,_cursorPos));
  switch (_plot.getTool()) {
  case DiffractogramPlot::TOOL_BACKGROUND:
    if (Qt::LeftButton == e->button())
      _plot.addBg(range);
    else
      _plot.remBg(range);
    break;

  case DiffractogramPlot::TOOL_PEAK_REGION:
    _plot.setNewReflRange(range);
    break;

  default:
    break;
  }
}

void DiffractogramPlotOverlay::mouseMoveEvent(QMouseEvent* e) {
  updateCursorRegion();
  _cursorPos = qBound(_marginLeft,e->x(),width()-_marginRight);
  updateCursorRegion();
  if (_mouseDown) update();
}

void DiffractogramPlotOverlay::paintEvent(QPaintEvent*) {
  QPainter painter(this);
  QRect g = geometry();

  if (_mouseDown) {
    g.setLeft(qMin(_mouseDownPos,_cursorPos));
    g.setRight(qMax(_mouseDownPos,_cursorPos));

    painter.fillRect(g,_color);
  }

  if (_hasCursor) {
    QLineF cursor(_cursorPos,g.top(),_cursorPos,g.bottom());

    painter.setPen(Qt::red);
    painter.drawLine(cursor);
  }
}

void DiffractogramPlotOverlay::updateCursorRegion() {
  auto g = geometry();
  // updating 2 pixels seems to work both on Linux & Mac
  update(_cursorPos-1,g.top(),2,g.height());
}

DiffractogramPlot::DiffractogramPlot(TheHub& theHub,Diffractogram& diffractogram)
: RefHub(theHub), _diffractogram(diffractogram), _showBgFit(false) {
  _overlay = new DiffractogramPlotOverlay(*this);

  auto *ar = axisRect();

  // fix margins
  QFontMetrics fontMetrics(font());
  int em = fontMetrics.width('M'), ascent = fontMetrics.ascent();

  QMargins margins(3*em,ascent,em,2*ascent);
  ar->setAutoMargins(QCP::msNone);
  ar->setMargins(margins);
  _overlay->setMargins(margins.left(),margins.right());

  // colours
  setBackground(palette().background().color());
  ar->setBackground(Qt::white);

  // graphs in the "main" layer; in the display order
  _bgGraph             = addGraph();
  _dgramGraph          = addGraph();
  _dgramBgFittedGraph  = addGraph();

  _bgGraph->setPen(QPen(Qt::green,0));
  _dgramGraph->setPen(QPen(Qt::gray));
  _dgramBgFittedGraph->setPen(QPen(Qt::blue,2));

  // background regions
  addLayer("bg",layer("background"),QCustomPlot::limAbove);
  // reflections
  addLayer("refl",layer("main"),QCustomPlot::limAbove);
  // reflections
  addLayer("marks",layer("refl"),QCustomPlot::limAbove);

  setCurrentLayer("marks");

  _guesses = addGraph();
  _guesses->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 8));
  _guesses->setLineStyle(QCPGraph::lsNone);
  _guesses->setPen(QPen(Qt::darkGray));

  _fits = addGraph();
  _fits->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 8));
  _fits->setLineStyle(QCPGraph::lsNone);
  _fits->setPen(QPen(Qt::red));

  connect(&_theHub, &TheHub::reflectionData, [this](core::shp_Reflection reflection) {
    _guesses->clearData();
    _fits->clearData();
    if (reflection) {
      auto &fun = reflection->peakFunction();

      auto gp = fun.guessedPeak();
      _guesses->addData(gp.x,gp.y);
      auto gw2 = fun.guessedFWHM() / 2;
      _guesses->addData(gp.x-gw2,gp.y/2);
      _guesses->addData(gp.x+gw2,gp.y/2);

      auto fp = fun.fittedPeak();
      _fits->addData(fp.x,fp.y);
      auto fw2 = fun.fittedFWHM() / 2;
      _fits->addData(fp.x-fw2,fp.y/2);
      _fits->addData(fp.x+fw2,fp.y/2);
    }
  });

  connect(_theHub.actions.fitShow, &QAction::toggled, [this](bool on) {
    _showBgFit = on;
    updateBg();
  });

  _tool = TOOL_NONE;
}

void DiffractogramPlot::setTool(Tool tool_) {
  _tool = tool_;
  updateBg();
}

void DiffractogramPlot::plot(
  core::rcCurve dgram, core::rcCurve dgramBgFitted, core::rcCurve bg,
  core::curve_vec const& refls, uint currReflIndex
) {
  if (dgram.isEmpty()) {
    xAxis->setVisible(false);
    yAxis->setVisible(false);

    _bgGraph->clearData();
    _dgramGraph->clearData();
    _dgramBgFittedGraph->clearData();

    clearReflLayer();

  } else {
    auto tthRange   = dgram.rgeX();
    bool fixedIntensityScale = _theHub.fixedIntensityScale;

    core::Range intenRange;
//    if (fixedIntensityScale) { TODO lens
//      auto max = _diffractogram.getDataset()->rgeInten(true).max;
//      // heuristics; to calculate this precisely would require much more computation
//      intenRange = core::Range(-max/30,max/3);
//    } else {
      intenRange = dgramBgFitted.rgeY();
      intenRange.extendBy(dgram.rgeY());
//    }

    xAxis->setRange(tthRange.min,tthRange.max);
    yAxis->setRange(qMin(0.,intenRange.min),intenRange.max);
    xAxis->setVisible(true);
    yAxis->setVisible(true);

    if (_showBgFit) {
      _bgGraph->setData(bg.xs(), bg.ys());
    } else {
      _bgGraph->clearData();
    }

    _dgramGraph->setData(dgram.xs(), dgram.ys());
    _dgramBgFittedGraph->setData(dgramBgFitted.xs(), dgramBgFitted.ys());

    clearReflLayer();
    setCurrentLayer("refl");

    for_i (refls.count()) {
      auto &r = refls[i];
      auto *graph = addGraph(); _reflGraph.append(graph);
      graph->setPen(QPen(Qt::green,i==(int)currReflIndex ? 2 : 1));
      graph->setData(r.xs(), r.ys());
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
  _theHub.bgRanges().clear();
  updateBg();
}

void DiffractogramPlot::addBg(core::rcRange range) {
  if (_theHub.bgRanges().add(range)) updateBg();
}

void DiffractogramPlot::remBg(core::rcRange range) {
  if (_theHub.bgRanges().rem(range)) updateBg();
}

void DiffractogramPlot::setNewReflRange(core::rcRange range) {
  _diffractogram.setCurrReflNewRange(range);
  updateBg();
}

void DiffractogramPlot::updateBg() {
  clearItems();

  switch (_tool) {
  default:
    break;
  case TOOL_BACKGROUND: {
    core::rcRanges rs = _theHub.bgRanges();
    for_i (rs.count()) addBgItem(rs.at(i));
    break;
  }
  case TOOL_PEAK_REGION:
    addBgItem(_diffractogram.currReflRange());
    break;
  }

  _diffractogram.renderDataset();
}

void DiffractogramPlot::clearReflLayer() {
  for (auto g: _reflGraph) removeGraph(g);
  _reflGraph.clear();
}

void DiffractogramPlot::addBgItem(core::rcRange range) {
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
  _overlay->setGeometry(0,0,size.width(),size.height());
}

//------------------------------------------------------------------------------

Diffractogram::Diffractogram(TheHub& theHub)
: super(EMPTY_STR,theHub,Qt::Vertical), _dataset(nullptr)
, _currReflIndex(-1) {
  _box->addWidget((_plot = new DiffractogramPlot(_theHub,*this)));

  connect(&_theHub, &TheHub::datasetSelected, [this](core::shp_Dataset dataset) {
    setDataset(dataset);
  });

  connect(&_theHub, &TheHub::geometryChanged, [this]() {
    renderDataset();
  });

  connect(&_theHub, &TheHub::correctionEnabled, [this]() {
    renderDataset();
  });

  connect(&_theHub, &TheHub::displayChange, [this]() {
    renderDataset();
  });

  connect(&_theHub, &TheHub::backgroundPolynomialDegree, [this](uint degree) {
    _theHub.bgPolynomialDegree() = degree; // keep session up-to-date
    renderDataset();
  });

  connect(&_theHub, &TheHub::normChanged, [this]() {
    renderDataset();
  });

  // REVIEW all these connects
  connect(_theHub.actions.fitBgClear, &QAction::triggered, [this]() {
    _plot->clearBg();
  });

  connect(&_theHub, &TheHub::fittingTab, [this](int index) {
    bool on = _theHub.actions.fitTool->isChecked();
    switch (index) {
    case TheHub::TAB_BACKGROUND:
      _plot->setTool(on ? DiffractogramPlot::TOOL_BACKGROUND : DiffractogramPlot::TOOL_NONE);
      break;
    case TheHub::TAB_REFLECTIONS:
      _plot->setTool(on ? DiffractogramPlot::TOOL_PEAK_REGION : DiffractogramPlot::TOOL_NONE);
      break;
    }
  });

  connect(_theHub.actions.fitTool, &QAction::toggled, [this](bool on) {
    _plot->setTool(on ? (0==_theHub.fittingTab__ ? DiffractogramPlot::TOOL_BACKGROUND:DiffractogramPlot::TOOL_PEAK_REGION) : DiffractogramPlot::TOOL_NONE);
  });

  connect(&_theHub, &TheHub::reflectionSelected, [this](core::shp_Reflection reflection) {
    _currentReflection = reflection;
    _plot->updateBg();
  });

  connect(&_theHub, &TheHub::reflectionValues, [this](core::rcRange range, core::rcXY peak, qreal fwhm, bool withGuesses) {
    if (_currentReflection) {
      _currentReflection->setRange(range);
      if (withGuesses)
        _currentReflection->invalidateGuesses();
      else {
        _currentReflection->setGuessPeak(peak);
        _currentReflection->setGuessFWHM(fwhm);
      }
      _plot->updateBg();
    }
  });

  _theHub.actions.fitShow->setChecked(true);
}

void Diffractogram::renderDataset() {
  calcDgram();
  calcBackground();
  calcReflections();

  _plot->plot(_dgram,_dgramBgFitted,_bg,_refls,_currReflIndex);
}

void Diffractogram::setDataset(core::shp_Dataset dataset_) {
  _dataset = dataset_;
  renderDataset();
}

void Diffractogram::calcDgram() { // TODO is like getDgram00 w useCut==true, normalize==false
  _dgram.clear();

  if (!_dataset) return;

  auto &map = _theHub.angleMap(*_dataset);
  _dgram = _theHub.lens(*_dataset)->makeCurve(map.rgeGamma(), map.rgeTth());
}

void Diffractogram::calcBackground() {
  _bg.clear(); _dgramBgFitted.clear();

  auto bgPolynomial = core::fit::fitPolynomial(
    _theHub.bgPolynomialDegree(),_dgram,_theHub.bgRanges());
  auto& tth   = _dgram.xs();
  auto& inten = _dgram.ys();
  for_i (_dgram.count()) {
    qreal x = tth[i], y = bgPolynomial.y(x);
    _bg.append(x,y);
    _dgramBgFitted.append(x,inten[i] - y);
  }
}

void Diffractogram::setCurrReflNewRange(core::rcRange range) {
  if (_currentReflection) {
    _currentReflection->setRange(range);
    _currentReflection->invalidateGuesses();
  }
}

core::Range Diffractogram::currReflRange() const {
  return _currentReflection ? _currentReflection->range() : core::Range();
}

void Diffractogram::calcReflections() {
  _refls.clear(); _currReflIndex = -1;

  auto rs = _theHub.reflections();
  for_i (rs.count()) {
    auto &r = rs[i];
    if (r == _currentReflection)
      _currReflIndex = i;

    auto range = r->range();
    if (range.min < range.max) {
      auto &fun = r->peakFunction();
      core::fit::fit(fun,_dgramBgFitted,range);

      auto &tth   = _dgramBgFitted.xs();

      core::Curve c;

      for_i (_dgramBgFitted.count()) {
        qreal x = tth[i];
        if (range.contains(x)) {
          c.append(x,fun.y(x));
        }
      }

      _refls.append(c);
    }
  }

  _theHub.setReflectionData(_currentReflection);
}

//------------------------------------------------------------------------------
}}
// eof
