// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      panel_diffractogram.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Antti Soininen, Jan Burle, Rebecca Brydon
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "panel_diffractogram.h"
#include "core_lens.h"
#include "thehub.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

DiffractogramPlotOverlay::DiffractogramPlotOverlay(DiffractogramPlot& plot_)
: super(&plot_), plot_(plot_), hasCursor_(false), mouseDown_(false)
, cursorPos_(0), mouseDownPos_(0)
{
  setMouseTracking(true);
  setMargins(0, 0);

  remColor_  = QColor(0xf8, 0xf8, 0xff, 0x90);
  bgColor_   = QColor(0x98, 0xfb, 0x98, 0x70);
  reflColor_ = QColor(0x87, 0xce, 0xfa, 0x70);
}

void DiffractogramPlotOverlay::setMargins(int left, int right) {
  marginLeft_  = left;
  marginRight_ = right;
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
  mouseDown_    = true;
  addColor_     = (eFittingTab::BACKGROUND == plot_.selectedFittingTab())
                      ? bgColor_: reflColor_;
  color_ = Qt::LeftButton == e->button() ? addColor_ : remColor_;
  update();
}

void DiffractogramPlotOverlay::mouseReleaseEvent(QMouseEvent* e) {
  mouseDown_ = false;
  update();

  core::Range range(plot_.fromPixels(mouseDownPos_, cursorPos_));
  switch (plot_.getTool()) {
  case DiffractogramPlot::eTool::BACKGROUND:
    if (Qt::LeftButton == e->button())
      plot_.addBg(range);
    else
      plot_.remBg(range);
    break;

  case DiffractogramPlot::eTool::PEAK_REGION:
    plot_.setNewReflRange(range);
    break;

  default:
    break;
  }
}

void DiffractogramPlotOverlay::mouseMoveEvent(QMouseEvent* e) {
  updateCursorRegion();
  cursorPos_ = qBound(marginLeft_, e->x(), width() - marginRight_);
  updateCursorRegion();
  if (mouseDown_) update();
}

void DiffractogramPlotOverlay::paintEvent(QPaintEvent*) {
  QPainter painter(this);
  QRect    g = geometry();

  if (mouseDown_) {
    g.setLeft(qMin(mouseDownPos_, cursorPos_));
    g.setRight(qMax(mouseDownPos_, cursorPos_));

    painter.fillRect(g, color_);
  }

  if (hasCursor_) {
    QLineF cursor(cursorPos_, g.top(), cursorPos_, g.bottom());

    painter.setPen(Qt::red);
    painter.drawLine(cursor);
  }
}

void DiffractogramPlotOverlay::updateCursorRegion() {
  auto g = geometry();
  // updating 2 pixels seems to work both on Linux & Mac
  update(cursorPos_ - 1, g.top(), 2, g.height());
}

DiffractogramPlot::DiffractogramPlot(TheHub& hub, Diffractogram& diffractogram)
: RefHub(hub), diffractogram_(diffractogram), showBgFit_(false)
{
  overlay_ = new DiffractogramPlotOverlay(*this);

  bgRgeColor_   = QColor(0x98, 0xfb, 0x98, 0x50);
  reflRgeColor_ = QColor(0x87, 0xce, 0xfa, 0x50);

  auto* ar = axisRect();

  // fix margins
  QFontMetrics fontMetrics(font());
  int          em = fontMetrics.width('M'), ascent = fontMetrics.ascent();

  QMargins margins(3 * em, ascent, em, 2 * ascent);
  ar->setAutoMargins(QCP::msNone);
  ar->setMargins(margins);
  overlay_->setMargins(margins.left(), margins.right());

  // colours
  setBackground(palette().background().color());
  ar->setBackground(Qt::white);

  // graphs in the "main" layer; in the display order
  bgGraph_            = addGraph();
  dgramGraph_         = addGraph();
  dgramBgFittedGraph_ = addGraph();

  bgGraph_->setPen(QPen(QColor(0x21, 0xa1, 0x21, 0xff), 2));
  dgramGraph_->setPen(QPen(Qt::gray));
  dgramBgFittedGraph_->setPen(QPen(Qt::black, 2));

  // background regions
  addLayer("bg", layer("background"), QCustomPlot::limAbove);
  // reflections
  addLayer("refl", layer("main"), QCustomPlot::limAbove);
  // reflections
  addLayer("marks", layer("refl"), QCustomPlot::limAbove);

  setCurrentLayer("marks");

  guesses_ = addGraph();
  guesses_->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 8));
  guesses_->setLineStyle(QCPGraph::lsNone);
  guesses_->setPen(QPen(Qt::darkGray));

  fits_ = addGraph();
  fits_->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 8));
  fits_->setLineStyle(QCPGraph::lsNone);
  fits_->setPen(QPen(Qt::red));

  onSigReflectionData([this](core::shp_Reflection reflection) {
    guesses_->clearData();
    fits_->clearData();

    if (reflection && diffractogram_.dataset()) {
      auto& fun = reflection->peakFunction();

      auto gp = fun.guessedPeak();
      guesses_->addData(gp.x, gp.y);
      auto gw2 = fun.guessedFWHM() / 2;
      guesses_->addData(gp.x - gw2, gp.y / 2);
      guesses_->addData(gp.x + gw2, gp.y / 2);

      auto fp = fun.fittedPeak();
      fits_->addData(fp.x, fp.y);
      auto fw2 = fun.fittedFWHM() / 2;
      fits_->addData(fp.x - fw2, fp.y / 2);
      fits_->addData(fp.x + fw2, fp.y / 2);
    }
  });

  connect(hub_.actions.fitBgShow, &QAction::toggled, [this](bool on) {
    showBgFit_ = on;
    updateBg();
  });

  onSigBgChanged([this]() {
    updateBg();
  });

  tool_ = eTool::NONE;
}

void DiffractogramPlot::setTool(eTool tool) {
  tool_ = tool;
  updateBg();
}

void DiffractogramPlot::plot(core::rcCurve dgram, core::rcCurve dgramBgFitted,
                             core::rcCurve bg, core::curve_vec const& refls,
                             uint currReflIndex)
{
  if (dgram.isEmpty()) {
    xAxis->setVisible(false);
    yAxis->setVisible(false);

    bgGraph_->clearData();
    dgramGraph_->clearData();
    dgramBgFittedGraph_->clearData();

    clearReflLayer();

  } else {
    auto tthRange = dgram.rgeX();

    core::Range intenRange;
    if (hub_.isFixedIntenDgramScale()) {
      ENSURE(!diffractogram_.dataset().isNull())
      auto lens = hub_.lens(*diffractogram_.dataset());
      auto max  = lens->rgeInten(hub_.isFixedIntenDgramScale()).max;
      // heuristics; to calculate this precisely would require much more
      // computation
      intenRange = core::Range(-max / 30, max / 3);
    } else {
      intenRange = dgramBgFitted.rgeY();
      intenRange.extendBy(dgram.rgeY());
    }

    xAxis->setRange(tthRange.min, tthRange.max);
    yAxis->setRange(qMin(0., intenRange.min), intenRange.max);
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
      auto& r     = refls[i];
      auto* graph = addGraph();
      reflGraph_.append(graph);
      graph->setPen(QPen(Qt::green, i == (int)currReflIndex ? 2 : 1));
      graph->setData(r.xs(), r.ys());
    }
  }

  replot();
}

core::Range DiffractogramPlot::fromPixels(int pix1, int pix2) {
  return core::Range::safeFrom(xAxis->pixelToCoord(pix1),
                               xAxis->pixelToCoord(pix2));
}

void DiffractogramPlot::clearBg() {
  hub_.setBgRanges(core::Ranges());
}

void DiffractogramPlot::addBg(core::rcRange range) {
  hub_.addBgRange(range);
}

void DiffractogramPlot::remBg(core::rcRange range) {
  hub_.remBgRange(range);
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
  case eTool::BACKGROUND: {
    core::rcRanges rs = hub_.bgRanges();
    for_i (rs.count())
      addBgItem(rs.at(i));
    break;
  }
  case eTool::PEAK_REGION:
    addBgItem(diffractogram_.currReflRange());
    break;
  }

  diffractogram_.render();
}

void DiffractogramPlot::clearReflLayer() {
  for (auto g : reflGraph_)
    removeGraph(g);
  reflGraph_.clear();
}

eFittingTab DiffractogramPlot::selectedFittingTab() {
  return hub_.fittingTab();
}

void DiffractogramPlot::addBgItem(core::rcRange range) {
  setCurrentLayer("bg");

  QColor color;
  switch (hub_.fittingTab()) {
  case eFittingTab::BACKGROUND:
    color = bgRgeColor_;
    break;
  case eFittingTab::REFLECTIONS:
    color = reflRgeColor_;
    break;
  }

  auto ir = new QCPItemRect(this);
  ir->setPen(QPen(color));
  ir->setBrush(QBrush(color));
  auto br = ir->bottomRight;
  br->setTypeY(QCPItemPosition::ptViewportRatio);
  br->setCoords(range.max, 1);
  auto tl = ir->topLeft;
  tl->setTypeY(QCPItemPosition::ptViewportRatio);
  tl->setCoords(range.min, 0);
  addItem(ir);
}

void DiffractogramPlot::resizeEvent(QResizeEvent* e) {
  super::resizeEvent(e);
  auto size = e->size();
  overlay_->setGeometry(0, 0, size.width(), size.height());
}

//------------------------------------------------------------------------------

Diffractogram::Diffractogram(TheHub& hub)
: super(hub, Qt::Vertical), dataset_(nullptr), currReflIndex_(-1)
{
  box_->addWidget((plot_ = new DiffractogramPlot(hub_, *this)));
  auto hb = hbox();
  box_->addLayout(hb);
  hb->addWidget(check("all datasets", hub_.actions.combinedDgram));
  hb->addWidget(check("fixed scale", hub_.actions.fixedIntenDgramScale));
  hb->addStretch();

  onSigDatasetSelected([this](core::shp_Dataset dataset) {
    setDataset(dataset);
  });

  onSigGeometryChanged([this]() {
    render();
  });

  onSigCorrEnabled([this]() {
    render();
  });

  onSigDisplayChanged([this]() {
    render();
  });

  onSigBgChanged([this]() {
    render();
  });

  onSigNormChanged([this]() {
    render();
  });

  // REVIEW all these connects
  connect(hub_.actions.fitBgClear, &QAction::triggered,
          [this]() { plot_->clearBg(); });

  onSigFittingTab([this](eFittingTab tab) {
    bool on = hub_.actions.fitRegions->isChecked();

    switch (tab) {
    case eFittingTab::BACKGROUND:
      hub_.actions.fitRegions
        ->text("Select background regions", true)
        .icon(":/icon/bgRegion");

      plot_->setTool(on ? DiffractogramPlot::eTool::BACKGROUND
                        : DiffractogramPlot::eTool::NONE);
      break;
    case eFittingTab::REFLECTIONS:
      hub_.actions.fitRegions
        ->text("Select reflection region", true)
        .icon(":/icon/reflRegion");

      plot_->setTool(on ? DiffractogramPlot::eTool::PEAK_REGION
                        : DiffractogramPlot::eTool::NONE);
      break;
    }
  });

  connect(hub_.actions.fitRegions, &QAction::toggled, [this](bool on) {
    DiffractogramPlot::eTool tool;

    switch (hub_.fittingTab()) {
    case eFittingTab::BACKGROUND:
      tool = DiffractogramPlot::eTool::BACKGROUND;
      break;
    case eFittingTab::REFLECTIONS:
      tool = DiffractogramPlot::eTool::PEAK_REGION;
      break;
    }

    plot_->setTool(on ? tool : DiffractogramPlot::eTool::NONE);
  });

  onSigReflectionSelected([this](core::shp_Reflection reflection) {
    currentReflection_ = reflection;
    plot_->updateBg();
  });

  onSigReflectionValues([this](core::rcRange range, core::rcXY peak, qreal fwhm,
                               bool withGuesses) {
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

  hub_.actions.fitBgShow->setChecked(true);
}

void Diffractogram::render() {
  calcDgram();
  calcBackground();
  calcReflections();

  plot_->plot(dgram_, dgramBgFitted_, bg_, refls_, currReflIndex_);
}

void Diffractogram::setDataset(core::shp_Dataset dataset) {
  dataset_ = dataset;
  render();
}

void Diffractogram::calcDgram() {
  dgram_.clear();

  if (!dataset_) return;

  auto& map = hub_.angleMap(*dataset_);
  if (hub_.isCombinedDgram())
    dgram_ = hub_.lens(*dataset_)->makeAvgCurve();
  else
    dgram_ = hub_.lens(*dataset_)->makeCurve(map.rgeGamma(), map.rgeTth());
}

void Diffractogram::calcBackground() {
  bg_.clear();
  dgramBgFitted_.clear();

  auto bgPolynom =
      core::fit::Polynom::fromFit(hub_.bgPolyDegree(), dgram_, hub_.bgRanges());

  for_i (dgram_.count()) {
    qreal x = dgram_.x(i), y = bgPolynom.y(x);
    bg_.append(x, y);
    dgramBgFitted_.append(x, dgram_.y(i) - y);
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
  refls_.clear();
  currReflIndex_ = -1;

  auto rs = hub_.reflections();
  for_i (rs.count()) {
    auto& r = rs[i];
    if (r == currentReflection_)
      currReflIndex_ = i;

    r->fit(dgramBgFitted_);
    auto &rge = r->range();
    auto &fun = r->peakFunction();

    core::Curve c;

    for_i (dgramBgFitted_.count()) {
      qreal x = dgramBgFitted_.x(i);
      if (rge.contains(x))
        c.append(x, fun.y(x));
    }

    refls_.append(c);
  }

  tellReflectionData(currentReflection_);
}

//------------------------------------------------------------------------------
}}
// eof
