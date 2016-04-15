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
: super(&plot_), plot_(plot_), hasCursor_(false), mouseDown_(false), cursorPos_(0)
, mouseDownPos_(0) {

  setMouseTracking(true);
  setMargins(0,0);

  addColor_ = QColor(0xff,0xf0,0xf0,0x80);
  remColor_ = QColor(0xf0,0xf0,0xff,0x80);
}

void DiffractogramPlotOverlay::setMargins(int left, int right) {
  marginLeft_ = left; marginRight_ = right;
}

void DiffractogramPlotOverlay::enterEvent(QEvent*) {
  hasCursor_ = true;
  updateCursorRegion();
}

void DiffractogramPlotOverlay::leaveEvent(QEvent*) {
  hasCursor_ = false;
  updateCursorRegion();
}

void DiffractogramPlotOverlay::mousePressEvent(QMouseEvent* e) {
  mouseDownPos_ = cursorPos_;
  mouseDown_ = true;
  color_ = Qt::LeftButton == e->button() ? addColor_ : remColor_;
  update();
}

void DiffractogramPlotOverlay::mouseReleaseEvent(QMouseEvent* e) {
  mouseDown_ = false;
  update();

  core::Range range(plot_.fromPixels(mouseDownPos_,cursorPos_));
  switch (plot_.getTool()) {
  case DiffractogramPlot::TOOL_BACKGROUND:
    if (Qt::LeftButton == e->button())
      plot_.addBg(range);
    else
      plot_.remBg(range);
    break;

  case DiffractogramPlot::TOOL_PEAK_REGION:
    plot_.setNewReflRange(range);
    break;

  default:
    break;
  }
}

void DiffractogramPlotOverlay::mouseMoveEvent(QMouseEvent* e) {
  updateCursorRegion();
  cursorPos_ = qBound(marginLeft_,e->x(),width()-marginRight_);
  updateCursorRegion();
  if (mouseDown_) update();
}

void DiffractogramPlotOverlay::paintEvent(QPaintEvent*) {
  QPainter painter(this);
  QRect g = geometry();

  if (mouseDown_) {
    g.setLeft(qMin(mouseDownPos_,cursorPos_));
    g.setRight(qMax(mouseDownPos_,cursorPos_));

    painter.fillRect(g,color_);
  }

  if (hasCursor_) {
    QLineF cursor(cursorPos_,g.top(),cursorPos_,g.bottom());

    painter.setPen(Qt::red);
    painter.drawLine(cursor);
  }
}

void DiffractogramPlotOverlay::updateCursorRegion() {
  auto g = geometry();
  // updating 2 pixels seems to work both on Linux & Mac
  update(cursorPos_-1,g.top(),2,g.height());
}

DiffractogramPlot::DiffractogramPlot(TheHub& theHub,Diffractogram& diffractogram)
: RefHub(theHub), diffractogram_(diffractogram), showBgFit_(false) {
  overlay_ = new DiffractogramPlotOverlay(*this);

  auto *ar = axisRect();

  // fix margins
  QFontMetrics fontMetrics(font());
  int em = fontMetrics.width('M'), ascent = fontMetrics.ascent();

  QMargins margins(3*em,ascent,em,2*ascent);
  ar->setAutoMargins(QCP::msNone);
  ar->setMargins(margins);
  overlay_->setMargins(margins.left(),margins.right());

  // colours
  setBackground(palette().background().color());
  ar->setBackground(Qt::white);

  // graphs in the "main" layer; in the display order
  bgGraph_             = addGraph();
  dgramGraph_          = addGraph();
  dgramBgFittedGraph_  = addGraph();

  bgGraph_->setPen(QPen(Qt::green,0));
  dgramGraph_->setPen(QPen(Qt::gray));
  dgramBgFittedGraph_->setPen(QPen(Qt::blue,2));

  // background regions
  addLayer("bg",layer("background"),QCustomPlot::limAbove);
  // reflections
  addLayer("refl",layer("main"),QCustomPlot::limAbove);
  // reflections
  addLayer("marks",layer("refl"),QCustomPlot::limAbove);

  setCurrentLayer("marks");

  guesses_ = addGraph();
  guesses_->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 8));
  guesses_->setLineStyle(QCPGraph::lsNone);
  guesses_->setPen(QPen(Qt::darkGray));

  fits_ = addGraph();
  fits_->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 8));
  fits_->setLineStyle(QCPGraph::lsNone);
  fits_->setPen(QPen(Qt::red));

  connect(&theHub_, &TheHub::reflectionData, [this](core::shp_Reflection reflection) {
    guesses_->clearData();
    fits_->clearData();
    if (reflection) {
      auto &fun = reflection->peakFunction();

      auto gp = fun.guessedPeak();
      guesses_->addData(gp.x,gp.y);
      auto gw2 = fun.guessedFWHM() / 2;
      guesses_->addData(gp.x-gw2,gp.y/2);
      guesses_->addData(gp.x+gw2,gp.y/2);

      auto fp = fun.fittedPeak();
      fits_->addData(fp.x,fp.y);
      auto fw2 = fun.fittedFWHM() / 2;
      fits_->addData(fp.x-fw2,fp.y/2);
      fits_->addData(fp.x+fw2,fp.y/2);
    }
  });

  connect(theHub_.actions.fitShow, &QAction::toggled, [this](bool on) {
    showBgFit_ = on;
    updateBg();
  });

  tool_ = TOOL_NONE;
}

void DiffractogramPlot::setTool(Tool tool) {
  tool_ = tool;
  updateBg();
}

void DiffractogramPlot::plot(
  core::rcCurve dgram, core::rcCurve dgramBgFitted, core::rcCurve bg,
  core::curve_vec const& refls, uint currReflIndex
) {
  if (dgram.isEmpty()) {
    xAxis->setVisible(false);
    yAxis->setVisible(false);

    bgGraph_->clearData();
    dgramGraph_->clearData();
    dgramBgFittedGraph_->clearData();

    clearReflLayer();

  } else {
    auto tthRange   = dgram.rgeX();

    core::Range intenRange;
//    if (fixedIntensityScale) { TODO use lens
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

    if (showBgFit_) {
      bgGraph_->setData(bg.xs(), bg.ys());
    } else {
      bgGraph_->clearData();
    }

    dgramGraph_->setData(dgram.xs(), dgram.ys());
    dgramBgFittedGraph_->setData(dgramBgFitted.xs(), dgramBgFitted.ys());

    clearReflLayer();
    setCurrentLayer("refl");

    for_i (refls.count()) {
      auto &r = refls[i];
      auto *graph = addGraph(); reflGraph_.append(graph);
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
  theHub_.bgRanges().clear();
  updateBg();
}

void DiffractogramPlot::addBg(core::rcRange range) {
  if (theHub_.bgRanges().add(range)) updateBg();
}

void DiffractogramPlot::remBg(core::rcRange range) {
  if (theHub_.bgRanges().rem(range)) updateBg();
}

void DiffractogramPlot::setNewReflRange(core::rcRange range) {
  diffractogram_.setCurrReflNewRange(range);
  updateBg();
}

void DiffractogramPlot::updateBg() {
  clearItems();

  switch (tool_) {
  default:
    break;
  case TOOL_BACKGROUND: {
    core::rcRanges rs = theHub_.bgRanges();
    for_i (rs.count()) addBgItem(rs.at(i));
    break;
  }
  case TOOL_PEAK_REGION:
    addBgItem(diffractogram_.currReflRange());
    break;
  }

  diffractogram_.renderDataset();
}

void DiffractogramPlot::clearReflLayer() {
  for (auto g: reflGraph_) removeGraph(g);
  reflGraph_.clear();
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
  overlay_->setGeometry(0,0,size.width(),size.height());
}

//------------------------------------------------------------------------------

Diffractogram::Diffractogram(TheHub& theHub)
: super(EMPTY_STR,theHub,Qt::Vertical), dataset_(nullptr)
, currReflIndex_(-1) {
  box_->addWidget((plot_ = new DiffractogramPlot(theHub_,*this)));

  connect(&theHub_, &TheHub::datasetSelected, [this](core::shp_Dataset dataset) {
    setDataset(dataset);
  });

  connect(&theHub_, &TheHub::geometryChanged, [this]() {
    renderDataset();
  });

  connect(&theHub_, &TheHub::correctionEnabled, [this]() {
    renderDataset();
  });

  connect(&theHub_, &TheHub::displayChange, [this]() {
    renderDataset();
  });

  connect(&theHub_, &TheHub::backgroundPolynomialDegree, [this](uint degree) {
    theHub_.bgPolynomialDegree() = degree; // keep session up-to-date
    renderDataset();
  });

  connect(&theHub_, &TheHub::normChanged, [this]() {
    renderDataset();
  });

  // REVIEW all these connects
  connect(theHub_.actions.fitBgClear, &QAction::triggered, [this]() {
    plot_->clearBg();
  });

  connect(&theHub_, &TheHub::fittingTab, [this](int index) {
    bool on = theHub_.actions.fitTool->isChecked();
    switch (index) {
    case TheHub::TAB_BACKGROUND:
      plot_->setTool(on ? DiffractogramPlot::TOOL_BACKGROUND : DiffractogramPlot::TOOL_NONE);
      break;
    case TheHub::TAB_REFLECTIONS:
      plot_->setTool(on ? DiffractogramPlot::TOOL_PEAK_REGION : DiffractogramPlot::TOOL_NONE);
      break;
    }
  });

  connect(theHub_.actions.fitTool, &QAction::toggled, [this](bool on) {
    plot_->setTool(on ? (0==theHub_.fittingTab__ ? DiffractogramPlot::TOOL_BACKGROUND:DiffractogramPlot::TOOL_PEAK_REGION) : DiffractogramPlot::TOOL_NONE);
  });

  connect(&theHub_, &TheHub::reflectionSelected, [this](core::shp_Reflection reflection) {
    currentReflection_ = reflection;
    plot_->updateBg();
  });

  connect(&theHub_, &TheHub::reflectionValues, [this](core::rcRange range, core::rcXY peak, qreal fwhm, bool withGuesses) {
    if (currentReflection_) {
      currentReflection_->setRange(range);
      if (withGuesses)
        currentReflection_->invalidateGuesses();
      else {
        currentReflection_->setGuessPeak(peak);
        currentReflection_->setGuessFWHM(fwhm);
      }
      plot_->updateBg();
    }
  });

  theHub_.actions.fitShow->setChecked(true);
}

void Diffractogram::renderDataset() {
  calcDgram();
  calcBackground();
  calcReflections();

  plot_->plot(dgram_,dgramBgFitted_,bg_,refls_,currReflIndex_);
}

void Diffractogram::setDataset(core::shp_Dataset dataset_) {
  dataset_ = dataset_;
  renderDataset();
}

void Diffractogram::calcDgram() { // TODO is like getDgram00 w useCut==true, normalize==false
  dgram_.clear();

  if (!dataset_) return;

  auto &map = theHub_.angleMap(*dataset_);
  dgram_ = theHub_.lens(*dataset_)->makeCurve(map.rgeGamma(), map.rgeTth());
}

void Diffractogram::calcBackground() {
  bg_.clear(); dgramBgFitted_.clear();

  auto bgPolynomial = core::fit::fitPolynomial(
    theHub_.bgPolynomialDegree(),dgram_,theHub_.bgRanges());
  auto& tth   = dgram_.xs();
  auto& inten = dgram_.ys();
  for_i (dgram_.count()) {
    qreal x = tth[i], y = bgPolynomial.y(x);
    bg_.append(x,y);
    dgramBgFitted_.append(x,inten[i] - y);
  }
}

void Diffractogram::setCurrReflNewRange(core::rcRange range) {
  if (currentReflection_) {
    currentReflection_->setRange(range);
    currentReflection_->invalidateGuesses();
  }
}

core::Range Diffractogram::currReflRange() const {
  return currentReflection_ ? currentReflection_->range() : core::Range();
}

void Diffractogram::calcReflections() {
  refls_.clear(); currReflIndex_ = -1;

  auto rs = theHub_.reflections();
  for_i (rs.count()) {
    auto &r = rs[i];
    if (r == currentReflection_)
      currReflIndex_ = i;

    auto range = r->range();
    if (range.min < range.max) {
      auto &fun = r->peakFunction();
      core::fit::fit(fun,dgramBgFitted_,range);

      auto &tth   = dgramBgFitted_.xs();

      core::Curve c;

      for_i (dgramBgFitted_.count()) {
        qreal x = tth[i];
        if (range.contains(x)) {
          c.append(x,fun.y(x));
        }
      }

      refls_.append(c);
    }
  }

  theHub_.setReflectionData(currentReflection_);
}

//------------------------------------------------------------------------------
}}
// eof
