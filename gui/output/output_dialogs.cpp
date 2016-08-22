// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      output_dialogs.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "output_dialogs.h"
#include "calc/calc_polefigure.h"
#include "thehub.h"
#include "types/type_models.h"

#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QProgressBar>
#include <QScrollArea>

namespace gui { namespace output {
//------------------------------------------------------------------------------

PanelReflection::PanelReflection(TheHub& hub) : super(hub, "Reflection") {
  auto g = grid();
  g->addWidget((cbRefl = comboBox(hub_.reflectionsModel.names())));
  g->addRowStretch();
}

PanelGammaSlices::PanelGammaSlices(TheHub& hub) : super(hub, "Gamma slices") {
  auto g = grid();

  g->addWidget(label("count"),               0, 0);
  g->addWidget((numSlices = spinCell(4, 1)), 0, 1);

  g->addWidget(label("degrees"),               1, 0);
  g->addWidget((stepGamma = spinCell(6, 0.0)), 1, 1);
  stepGamma->setReadOnly(true);

  g->addRowStretch();

  rgeGma_ = hub_.collectedDatasetsRgeGma();

  connect(numSlices, slot(QSpinBox,valueChanged,int), [this]() {
    updateValues();
  });
}

void PanelGammaSlices::updateValues() {
  stepGamma->setValue(rgeGma_.width() / numSlices->value());
}

PanelGammaRange::PanelGammaRange(TheHub& hub) : super(hub, "Gamma range") {
  auto g = grid();

  g->addWidget((cbLimitGamma = check("limit")),       0, 0, 1, 2);

  g->addWidget(label("min"),                          1, 0);
  g->addWidget((minGamma = spinCell(6, -180., 180.)), 1, 1);

  g->addWidget(label("max"),                          2, 0);
  g->addWidget((maxGamma = spinCell(6, -180., 180.)), 2, 1);

  g->addRowStretch();

  rgeGma_ = hub_.collectedDatasetsRgeGma();

  minGamma->setValue(rgeGma_.min);
  maxGamma->setValue(rgeGma_.max);

  connect(cbLimitGamma, &QCheckBox::toggled, [this]() {
    updateValues();
  });
}

// TODO when min/maxGamma updated -> reflect that in PanelGammaSlices

void PanelGammaRange::updateValues() {
  bool on = cbLimitGamma->isChecked();
  minGamma->setEnabled(on);
  maxGamma->setEnabled(on);
}

PanelPoints::PanelPoints(TheHub& hub) : super(hub, "Points") {
  auto g = grid();
  g->addWidget((rbCalc   = radioButton("calculated")),   0, 0);
  g->addWidget((rbInterp = radioButton("interpolated")), 1, 0);

  g->addRowStretch();
}

PanelInterpolation::PanelInterpolation(TheHub& hub) : super(hub, "Interpolation") {
  auto g = grid();

  g->addWidget(label("step α"), 0, 0, Qt::AlignRight);
  g->addWidget((stepAlpha = spinCell(6, 1., 30.)), 0, 1);
  g->addWidget(label("β"), 1, 0, Qt::AlignRight);
  g->addWidget((stepBeta = spinCell(6, 1., 30.)), 1, 1);
  g->addWidget(label("idw radius"), 2, 0, Qt::AlignRight);
  g->addWidget((idwRadius = spinCell(6, 0., 90.)), 2, 1);

  g->addWidget(label("avg. α max"), 0, 2, Qt::AlignRight);
  g->addWidget((avgAlphaMax = spinCell(6, 0., 90.)), 0, 3);
  g->addWidget(label("radius"), 1, 2, Qt::AlignRight);
  g->addWidget((avgRadius = spinCell(6, 0., 90.)), 1, 3);
  g->addWidget(label("inclusion %"), 2, 2, Qt::AlignRight);
  g->addWidget((avgThreshold = spinCell(6, 0, 100)), 2, 3);

  g->addRowStretch();
}

PanelDiagram::PanelDiagram(TheHub& hub) : super(hub, "Diagram") {
  auto tags = calc::ReflectionInfo::dataTags();
  for_i (data::Metadata::numAttributes(false) - data::Metadata::numAttributes(true))
    tags.removeLast(); // remove all tags that are not numbers

  auto g = grid();
  g->addWidget(label("y"),               0, 0);
  g->addWidget((yAxis = comboBox(tags)), 0, 1);
  g->addWidget(label("x"),               1, 0);
  g->addWidget((xAxis = comboBox(tags)), 1, 1);

  g->addRowStretch();
}

PanelFitError::PanelFitError(TheHub& hub) : super(hub, "Fit error") {
}

// REVIEW
#ifdef DEVELOP_REBECCA
  static str_lst errorUnits = {"absolute Error","Δ to other Fit"};
  static str_lst errorTypes = {"Intensity","Tth","Fwhm"};
  {
    box_->addWidget((gpFitError_ = new panel::GridPanel(hub,"Fit Error")));
    auto g = gpFitError_->grid();
    g->addWidget(cbErrorTypes_ = comboBox(errorTypes),0,0);

    g->addWidget(intensityFitError_ = new panel::FitErrorGridPannel(hub),1,0); // displayed ony one at same pos
    g->addWidget(tthFitError_       = new panel::FitErrorGridPannel(hub),1,0);
    g->addWidget(fwhmFitError_      = new panel::FitErrorGridPannel(hub),1,0);

    g->addRowStretch();
    g->setMargin(1);

    {
      auto g = intensityFitError_->grid();
      g->addWidget((intensityFitError_->cbErrorUnits = comboBox(errorUnits)),0,0);
      auto grb = gridLayout();
      grb->addWidget(intensityFitError_->rbAbs      = radioButton("Abs."),0,0);
      grb->addWidget(intensityFitError_->rbPercent  = radioButton("%"),0,1);
      g->addLayout(grb,1,0);
      g->addWidget((intensityFitError_->spFitError  = spinCell(6,0.,50.)),3,0);

      g->addColumnStretch();
      g->addRowStretch();
    }

    {
      auto g = tthFitError_->grid();
      g->addWidget((tthFitError_->cbErrorUnits = comboBox(errorUnits)),0,0);
      auto grb = gridLayout();
      grb->addWidget(tthFitError_->rbAbs = radioButton("Abs."),0,0);
      grb->addWidget(tthFitError_->rbPercent = radioButton("%"),0,1);
      g->addLayout(grb,1,0);
      g->addWidget((tthFitError_->spFitError = spinCell(6,0.,50.)),3,0);

      g->addColumnStretch();
      g->addRowStretch();
    }

    {
      auto g = fwhmFitError_->grid();
      g->addWidget((fwhmFitError_->cbErrorUnits = comboBox(errorUnits)),0,0);
      auto grb = gridLayout();
      grb->addWidget(fwhmFitError_->rbAbs = radioButton("Abs."),0,0);
      grb->addWidget(fwhmFitError_->rbPercent= radioButton("%"),0,1);
      g->addLayout(grb,1,0);
      g->addWidget((fwhmFitError_->spFitError = spinCell(6,0.,50.)),3,0);

      g->addColumnStretch();
      g->addRowStretch();
    }

    intensityFitError_->show();
    tthFitError_->hide();
    fwhmFitError_->hide();

  }

    enum FitErrorTypes {
    INTENSITY_ERROR, TTH_ERROR, FWHM_ERROR,
  };
  enum FitErrorUnits {
    ABSOLUTE, DELTA_TO_NEXT,
  };

  connect(cbErrorTypes_, slot(QComboBox,currentIndexChanged,int),[this](int index) {
    switch (index) {
    case INTENSITY_ERROR:
      intensityFitError_->show();
      tthFitError_->hide();
      fwhmFitError_->hide();
      break;
    case TTH_ERROR:
      intensityFitError_->hide();
      tthFitError_->show();
      fwhmFitError_->hide();
      break;
    case FWHM_ERROR:
      intensityFitError_->hide();
      tthFitError_->show();
      fwhmFitError_->hide();
      break;
    }
  });

  auto setRbEnabled = [this](int index, QRadioButton* rb) {
    switch (index) {
    case ABSOLUTE:
      rb->setEnabled(false);
      break;
    case DELTA_TO_NEXT:
      rb->setEnabled(true);
      break;
    }
  };

  connect(intensityFitError_->cbErrorUnits, slot(QComboBox,currentIndexChanged,int),[this,setRbEnabled](int index) {
    setRbEnabled(index,intensityFitError_->rbPercent);
    if (!intensityFitError_->rbPercent->isEnabled()) {
      intensityFitError_->rbPercent->setChecked(false);
      intensityFitError_->rbAbs->setChecked(true);
    }
  });

  connect(tthFitError_->cbErrorUnits, slot(QComboBox,currentIndexChanged,int),[this,setRbEnabled](int index){
    setRbEnabled(index,tthFitError_->rbPercent);
    if (!tthFitError_->rbPercent->isEnabled()) {
      tthFitError_->rbPercent->setChecked(false);
      tthFitError_->rbAbs->setChecked(true);
    }

  });

  connect(fwhmFitError_->cbErrorUnits, slot(QComboBox,currentIndexChanged,int),[this,setRbEnabled](int index){
    setRbEnabled(index,fwhmFitError_->rbPercent);
    if (!fwhmFitError_->rbPercent->isEnabled()) {
      fwhmFitError_->rbPercent->setChecked(false);
      fwhmFitError_->rbAbs->setChecked(true);
    }
  });

  intensityFitError_->rbPercent->setEnabled(false);
  tthFitError_->rbPercent->setEnabled(false);
  fwhmFitError_->rbPercent->setEnabled(false);

#endif

//------------------------------------------------------------------------------

Params::Params(TheHub& hub, ePanels panels)
: RefHub(hub)
, panelReflection(nullptr)
, panelGammaSlices(nullptr), panelGammaRange(nullptr), panelPoints(nullptr)
, panelInterpolation(nullptr), panelDiagram(nullptr)
{
  EXPECT(panels & GAMMA)

  setLayout((box_ = boxLayout(Qt::Horizontal)));

  if (REFLECTION & panels)
    box_->addWidget((panelReflection = new PanelReflection(hub)));

  if (GAMMA & panels) {
    box_->addWidget((panelGammaSlices = new PanelGammaSlices(hub)));
    box_->addWidget((panelGammaRange  = new PanelGammaRange(hub)));
  }

  if (POINTS & panels)
    box_->addWidget((panelPoints = new PanelPoints(hub)));

  if (INTERPOLATION & panels)
    box_->addWidget((panelInterpolation = new PanelInterpolation(hub)));

  if (DIAGRAM & panels)
    box_->addWidget((panelDiagram = new PanelDiagram(hub)));

  box_->setMargin(0);
  box_->addStretch();

  readSettings();
}

Params::~Params() {
  saveSettings();
}

static str SETTINGS_GROUP = "output";
static str const
  KEY_NUM_SLICES("num slices"),
  KEY_LIMIT_GAMMA("limit gamma"),

  KEY_INTERPOLATED("interpolated"),

  KEY_STEP_ALPHA("step alpha"),
  KEY_STEP_BETA("step beta"),
  KEY_IDW_RADIUS("idw radius"),

  KEY_AVG_ALPHA_MAX("avg alpha max"),
  KEY_AVG_RADIUS("avg radius"),
  KEY_AVG_THRESHOLD("avg threshold"),

  KEY_SAVE_DIR("save dir"),
  KEY_SAVE_FMT("save format");

void Params::readSettings() {
  Settings s(SETTINGS_GROUP);

  if (panelGammaSlices) {
    panelGammaSlices->numSlices->setValue(s.readReal(KEY_NUM_SLICES, 1));
  }

  if (panelGammaRange) {
    panelGammaRange->cbLimitGamma->setChecked(s.readBool(KEY_LIMIT_GAMMA, false));
  }

  if (panelPoints) {
    (s.readBool(KEY_INTERPOLATED, false)
        ? panelPoints->rbInterp : panelPoints->rbCalc)->setChecked(true);
  }

  if (panelInterpolation) {
    panelInterpolation->stepAlpha->setValue(s.readReal(KEY_STEP_ALPHA, 5));
    panelInterpolation->stepBeta->setValue(s.readReal(KEY_STEP_BETA, 5));
    panelInterpolation->idwRadius->setValue(s.readReal(KEY_IDW_RADIUS, 10));

    panelInterpolation->avgAlphaMax->setValue(s.readReal(KEY_AVG_ALPHA_MAX, 15));
    panelInterpolation->avgRadius->setValue(s.readReal(KEY_AVG_RADIUS, 5));
    panelInterpolation->avgThreshold->setValue(s.readReal(KEY_AVG_THRESHOLD, 100));
  }

  saveDir = s.readStr(KEY_SAVE_DIR);
  saveFmt = s.readStr(KEY_SAVE_FMT);

  if (panelGammaSlices)
    panelGammaSlices->updateValues();

  if (panelGammaRange)
    panelGammaRange->updateValues();
}

void Params::saveSettings() const {
  Settings s(SETTINGS_GROUP);

  if (panelGammaSlices) {
    s.saveReal(KEY_NUM_SLICES, panelGammaSlices->numSlices->value());
  }

  if (panelGammaRange) {
    s.saveBool(KEY_LIMIT_GAMMA,  panelGammaRange->cbLimitGamma->isChecked());
  }

  if (panelPoints) {
    s.saveBool(KEY_INTERPOLATED, panelPoints->rbInterp->isChecked());
  }

  if (panelInterpolation) {
    s.saveReal(KEY_STEP_ALPHA,    panelInterpolation->stepAlpha->value());
    s.saveReal(KEY_STEP_BETA,     panelInterpolation->stepBeta->value());
    s.saveReal(KEY_IDW_RADIUS,    panelInterpolation->idwRadius->value());

    s.saveReal(KEY_AVG_ALPHA_MAX, panelInterpolation->avgAlphaMax->value());
    s.saveReal(KEY_AVG_RADIUS,    panelInterpolation->avgRadius->value());
    s.saveReal(KEY_AVG_THRESHOLD, panelInterpolation->avgThreshold->value());
  }

  s.saveStr(KEY_SAVE_DIR, saveDir);
  s.saveStr(KEY_SAVE_FMT, saveFmt);
}

//------------------------------------------------------------------------------

class TableModel : public models::TableModel {
  CLS(TableModel) SUPER(models::TableModel)

public:
  TableModel(TheHub&, uint numCols_);

  int columnCount(rcIndex = models::ANY_INDEX) const;
  int rowCount(rcIndex = models::ANY_INDEX)    const;

  QVariant data(rcIndex, int) const;
  QVariant headerData(int, Qt::Orientation, int) const;

  void moveColumn(uint from, uint to);

  void setColumns(str_lst::rc headers, typ::cmp_vec::rc);
  str_lst const headers() { return headers_; }

  void setSortColumn(int);

  void clear();
  void addRow(typ::row_t::rc, bool sort = true);

  typ::row_t::rc row(uint);

  void sortData();

private:
  uint numCols_;
  int  sortColumn_;

  str_lst      headers_;
  uint_vec     colIndexMap_;
  typ::cmp_vec cmpFunctions_;

  typ::vec<typ::row_t> rows_;
};

//------------------------------------------------------------------------------

TableModel::TableModel(TheHub& hub, uint numColumns_)
: models::TableModel(hub), numCols_(numColumns_), sortColumn_(-1)
{
  colIndexMap_.resize(numCols_);
  for_i (numCols_)
    colIndexMap_[i] = i;
}

int TableModel::columnCount(rcIndex) const {
  return to_i(numCols_) + 1;
}

int TableModel::rowCount(rcIndex) const {
  return to_i(rows_.count());
}

// The first column contains row numbers. The rest numCols columns contain data.

QVariant TableModel::data(rcIndex index, int role) const {
  int indexRow = index.row(), indexCol = index.column();
  int numRows  = rowCount(),  numCols  = columnCount();

  if (indexCol < 0 || indexRow < 0)
    return QVariant();

  switch (role) {
  case Qt::DisplayRole:
    if (0 == indexCol)
      return indexRow + 1;  // row number, 1-based

    if (--indexCol < numCols && indexRow < numRows) {
      QVariant var = rows_.at(to_u(indexRow)).at(to_u(indexCol));
      if (typ::isReal(var) && qIsNaN(var.toDouble()))
        var = QVariant(); // hide nans
      return var;
    }

    break;

  case Qt::TextAlignmentRole:
    if (0 == indexCol)
      return Qt::AlignRight;

    if (--indexCol < numCols && indexRow < numRows) {
      QVariant const& var = rows_.at(to_u(indexRow)).at(to_u(indexCol));
      if (typ::isNumeric(var))
        return Qt::AlignRight;
    }

    return Qt::AlignLeft;

  default:
    break;
  }

  return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation, int role) const {
  if (section < 0 || headers_.count() < section)
    return QVariant();

  if (Qt::DisplayRole == role)
    return 0 == section ? "#" : headers_.at(to_u(section) - 1);

  return QVariant();
}

void TableModel::moveColumn(uint from, uint to) {
  EXPECT(from < colIndexMap_.count() && to < colIndexMap_.count())
  qSwap(colIndexMap_[from], colIndexMap_[to]);
}

void TableModel::setColumns(str_lst::rc headers, typ::cmp_vec::rc cmps) {
  EXPECT(to_u(headers.count()) == numCols_ && cmps.count() == numCols_)
  headers_ = headers;
  cmpFunctions_ = cmps;
}

void TableModel::setSortColumn(int col) {
  sortColumn_ = col;
}

void TableModel::clear() {
  beginResetModel();
  rows_.clear();
  endResetModel();
}

void TableModel::addRow(typ::row_t::rc row, bool sort) {
  rows_.append(row);
  if (sort)
    sortData();
}

typ::row_t::rc TableModel::row(uint index) {
  return rows_.at(index);
}

void TableModel::sortData() {
  // sort sortColumn first, then left-to-right
  auto cmpRows = [this](uint i, typ::row_t::rc r1, typ::row_t::rc r2) {
    i = colIndexMap_.at(i);
    return cmpFunctions_.at(i)(r1.at(i), r2.at(i));
  };

  auto cmp = [this, cmpRows](typ::row_t::rc r1, typ::row_t::rc r2) {
    if (sortColumn_ >= 0) {
      int c = cmpRows(to_u(sortColumn_), r1, r2);
      if (c < 0)
        return true;
      if (c > 0)
        return false;
    }

    for_i (numCols_) {
      if (to_i(i) != sortColumn_) {
        int c = cmpRows(i, r1, r2);
        if (c < 0)
          return true;
        if (c > 0)
          return false;
      }
    }

    return false;
  };

  beginResetModel();
  std::sort(rows_.begin(), rows_.end(), cmp);
  endResetModel();
}

//------------------------------------------------------------------------------

Tabs::Tabs(TheHub& hub) : super(hub) {
}

//------------------------------------------------------------------------------

Tab::Tab(TheHub& hub, Params& params) : RefHub(hub), params_(params) {
  setLayout((grid_ = gridLayout()));
}

//------------------------------------------------------------------------------

Table::Table(TheHub& hub, uint numDataColumns)
: RefHub(hub), model_(nullptr)
{
  model_.reset(new TableModel(hub_, numDataColumns));
  setModel(model_.ptr());
  setHeader(new QHeaderView(Qt::Horizontal));

  auto& h = *header();

  h.setSectionResizeMode(0, QHeaderView::Fixed);
  h.setSectionsMovable(true);
  h.setSectionsClickable(true);

  int w = QFontMetrics(h.font()).width("000000000");
  setColumnWidth(0, w);
}

void Table::setColumns(str_lst::rc headers, typ::cmp_vec::rc cmps) {
  model_->setColumns(headers, cmps);

  connect(header(), &QHeaderView::sectionMoved,
          [this](int /*logicalIndex*/, int oldVisualIndex, int newVisualIndex) {
            EXPECT(oldVisualIndex > 0 && newVisualIndex > 0)
            auto& h = *header();
            h.setSortIndicatorShown(false);
            model_->setSortColumn(-1);
            model_->moveColumn(to_u(oldVisualIndex - 1),
                               to_u(newVisualIndex - 1));
            model_->sortData();
          });

  connect(header(), &QHeaderView::sectionClicked, [this](int logicalIndex) {
    auto& h = *header();
    h.setSortIndicatorShown(true);
    h.setSortIndicator(logicalIndex, Qt::AscendingOrder);
    model_->setSortColumn(logicalIndex - 1);
    model_->sortData();
  });
}

const str_lst Table::headers() {
  return model_->headers();
}

void Table::clear() {
  model_->clear();
}

void Table::addRow(typ::row_t::rc row, bool sort) {
  model_->addRow(row, sort);
}

void Table::sortData() {
  model_->sortData();
}

uint Table::rowCount() const {
  return to_u(model_->rowCount());
}

const typ::row_t&Table::row(uint i) const {
  return model_->row(i);
}

//------------------------------------------------------------------------------

TabTable::TabTable(TheHub& hub, Params& params,
                   str_lst::rc headers, typ::cmp_vec::rc cmps)
: super(hub, params)
{
  EXPECT(to_u(headers.count()) == cmps.count())
  uint numCols = to_u(headers.count());

  grid_->setMargin(0);
  grid_->addWidget((table = new Table(hub_, numCols)), 0, 0);
  grid_->setColumnStretch(0, 1);

  table->setColumns(headers, cmps);

  for_i (numCols) {
    showcol_t item;
    item.name = headers.at(i);
    showCols_.append(item);
  }

  auto scrollArea = new QScrollArea;
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollArea->setWidget((showColumnsWidget_ = new ShowColsWidget(*table, showCols_)));

  grid_->addWidget(scrollArea, 0, 1);
}

//------------------------------------------------------------------------------

TabTable::ShowColsWidget::ShowColsWidget(Table& table, showcol_vec& showCols)
: table_(table), showCols_(showCols)
{
  using eReflAttr = calc::ReflectionInfo::eReflAttr;

  setLayout((box_ = vbox()));

  box_->addWidget((rbHidden_ = radioButton("")));
  rbHidden_->hide();

  box_->addWidget((rbAll_  = radioButton("all")));
  box_->addWidget((rbNone_ = radioButton("none")));
  box_->addWidget(rbInten_ = radioButton("Intensity"));
  box_->addWidget(rbTth_   = radioButton("2θ"));
  box_->addWidget(rbFWHM_  = radioButton("fwhm"));
  box_->addSpacing(8);

  for_i (showCols.count()) {
    auto& item = showCols[i];
    box_->addWidget((item.cb = check(item.name)));
  }

  auto all = [this]() {
    for (auto& col: showCols_)
      col.cb->setChecked(true);
  };

  auto none = [this]() {
    for (auto& col: showCols_)
      col.cb->setChecked(false);
  };

  auto showAlphaBeta = [this,none]() {
    none();
    showCols_.at(uint(eReflAttr::ALPHA)).cb->setChecked(true);
    showCols_.at(uint(eReflAttr::BETA)).cb->setChecked(true);
  };

  auto showInten = [this,showAlphaBeta]() {
    showAlphaBeta();
    showCols_.at(uint(eReflAttr::INTEN)).cb->setChecked(true);
  };

  auto showTth = [this,showAlphaBeta]() {
    showAlphaBeta();
    showCols_.at(uint(eReflAttr::TTH)).cb->setChecked(true);
  };

  auto showFWHM = [this,showAlphaBeta]() {
    showAlphaBeta();
    showCols_.at(uint(eReflAttr::FWHM)).cb->setChecked(true);
  };

  auto updateRbs = [this]() {
    bool isAll = true, isNone = true, isOther = false;
    uint nInten = 0, nTth = 0, nFwhm = 0;

    for_i (showCols_.count()) {
      if (!showCols_.at(i).cb->isChecked()) {
        isAll = false;
        continue;
      }

      isNone = false;

      switch (eReflAttr(i)) {
      case eReflAttr::ALPHA:
      case eReflAttr::BETA:
        ++nInten; ++nTth; ++nFwhm;
        break;
      case eReflAttr::INTEN:
        ++nInten;
        break;
      case eReflAttr::TTH:
        ++nTth;
        break;
      case eReflAttr::FWHM:
        ++nFwhm;
        break;
      default:
        isOther = true;
        break;
      }
    }

    rbHidden_->setChecked(true);
    rbNone_->setChecked(isNone);
    rbAll_->setChecked(isAll);

    uint const PRESET_SELECTION = 3;

    rbInten_->setChecked(!isOther && PRESET_SELECTION == nInten);
    rbTth_->setChecked(!isOther   && PRESET_SELECTION == nTth);
    rbFWHM_->setChecked(!isOther  && PRESET_SELECTION == nFwhm);
  };

  for_i (showCols_.count()) {
    auto cb = showCols_.at(i).cb;

    connect(cb, &QCheckBox::toggled, [this, updateRbs, i](bool on) {
      if (on)
        table_.showColumn(to_i(i) + 1);
      else
        table_.hideColumn(to_i(i) + 1);

      updateRbs();
    });
  }

  connect(rbAll_,   &QRadioButton::clicked, all);
  connect(rbNone_,  &QRadioButton::clicked, none);
  connect(rbInten_, &QRadioButton::clicked, showInten);
  connect(rbTth_,   &QRadioButton::clicked, showTth);
  connect(rbFWHM_,  &QRadioButton::clicked, showFWHM);

  rbAll_->click();
}

//------------------------------------------------------------------------------

static str const
  DAT_SFX(".dat"), DAT_SEP(" "),  // suffix, separator
  CSV_SFX(".csv"), CSV_SEP(", ");

TabSave::TabSave(TheHub& hub, Params& params, bool withTypes) : super(hub, params) {
  actBrowse = new TriggerAction("Browse...", this);
  actSave   = new TriggerAction("Save", this);

  str dir = params_.saveDir;
  if (!QDir(dir).exists())
    dir = QDir::current().absolutePath();

  auto gp = new panel::GridPanel(hub, "Destination");
  grid_->addWidget(gp, 0, 0);
  auto g = gp->grid();

  dir_  = new QLineEdit(dir);
  dir_->setReadOnly(true);

  file_ = new QLineEdit();

  g->addWidget(label("Save to folder:"), 0, 0, Qt::AlignRight);
  g->addWidget(dir_,                     0, 1);
  g->addWidget(textButton(actBrowse),    0, 2);

  g->addWidget(label("File name:"),      1, 0, Qt::AlignRight);
  g->addWidget(file_,                    1, 1);

  connect(actBrowse, &QAction::triggered, [this]() {
    str dir = QFileDialog::getExistingDirectory(this, "Select folder", dir_->text());
    if (!dir.isEmpty())
      dir_->setText((params_.saveDir = dir));
  });

  gp = new panel::GridPanel(hub, "File type");
  grid_->addWidget(gp, 0, 1);
  g = gp->grid();

  g->addWidget((rbDat_ = radioButton(DAT_SFX)), 0, 0);
  g->addWidget((rbCsv_ = radioButton(CSV_SFX)), 1, 0);

  connect(rbDat_, &QRadioButton::clicked, [this]() {
    params_.saveFmt = DAT_SFX;
  });

  connect(rbCsv_, &QRadioButton::clicked, [this]() {
    params_.saveFmt = CSV_SFX;
  });

  (CSV_SFX == params_.saveFmt ? rbCsv_ : rbDat_)->setChecked(true);

  gp->setVisible(withTypes);
}

str TabSave::filePath(bool withSuffix) {
  str dir  = dir_->text().trimmed();
  str file = file_->text().trimmed();

  if (dir.isEmpty() || file.isEmpty())
    return EMPTY_STR;
  
  str suffix;
  if (withSuffix)
    suffix = rbDat_->isChecked() ? DAT_SFX : CSV_SFX;

  return QFileInfo(dir + '/' + fileSetSuffix(suffix)).absoluteFilePath();
}

str TabSave::separator() const {
  return rbDat_->isChecked() ? DAT_SEP : CSV_SEP;
}

str TabSave::fileSetSuffix(rcstr suffix) {
  str file = file_->text().trimmed();
  if (!suffix.isEmpty()) {
    file = QFileInfo(file).completeBaseName();
    if (!file.isEmpty())
      file += suffix;
  }
  
  file_->setText(file);
  return file;
}


//------------------------------------------------------------------------------

Frame::Frame(TheHub& hub, rcstr title, Params* params, QWidget* parent)
: super(parent, Qt::Dialog), RefHub(hub)
{
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  setWindowTitle(title);
  setLayout((box_ = vbox()));

  EXPECT(params)

  box_->addWidget((params_ = params));
  box_->addWidget((tabs_   = new Tabs(hub)));
  box_->setStretch(box_->count() - 1, 1);

  auto hb = hbox();
  box_->addLayout(hb);

  actClose_       = new TriggerAction("Close",       this);
  actCalculate_   = new TriggerAction("Calculate",   this);
  actInterpolate_ = new TriggerAction("Interpolate", this);

  hb->addWidget((btnClose_ = textButton(actClose_)));
  hb->addStretch(1);
  hb->addWidget((pb_ = new QProgressBar));
  hb->setStretchFactor(pb_, 333);
  hb->addStretch(1);
  hb->addWidget((btnCalculate_  = textButton(actCalculate_)));
  hb->addWidget((btnInterpolate_= textButton(actInterpolate_)));

  pb_->hide();

  connect(actClose_, &QAction::triggered, [this]() {
    close();
  });

  connect(actCalculate_, &QAction::triggered, [this]() {
    calculate();
  });

  connect(actInterpolate_, &QAction::triggered, [this]() {
    interpolate();
  });

  auto updateDisplay = [this]() {
    displayReflection(getReflIndex(), getInterpolated());
  };

  if (params_->panelReflection) {
    connect(params_->panelReflection->cbRefl, slot(QComboBox,currentIndexChanged,int), [updateDisplay]() {
      updateDisplay();
    });
  }

  if (params_->panelPoints) {
    ENSURE(params_->panelReflection)
    connect(params_->panelPoints->rbInterp, &QRadioButton::toggled, [updateDisplay]() {
      updateDisplay();
    });
  }

  // tabs

  auto tabTable = new TabTable(hub, *params_,
     calc::ReflectionInfo::dataTags(), calc::ReflectionInfo::dataCmps());
  tabs_->addTab("Points").box().addWidget(tabTable);

  table_ = tabTable->table;

  uint reflCount = hub_.reflections().count();
  calcPoints_.resize(reflCount);
  interpPoints_.resize(reflCount);
}

void Frame::calculate() {
  TakesLongTime __;

  calcPoints_.clear();
  interpPoints_.clear();

  auto &reflections = hub_.reflections();
  if (!reflections.isEmpty()) {
    uint reflCount = reflections.count();

    auto ps = params_->panelGammaSlices;
    ENSURE(ps)

    pint gammaSlices = pint(ps->numSlices->value());

    auto pr = params_->panelGammaRange;
    ENSURE(pr)

    typ::Range rgeGamma;
    if (pr->cbLimitGamma->isChecked())
      rgeGamma.safeSet(pr->minGamma->value(), pr->maxGamma->value());

    Progress progress(reflCount * hub_.numCollectedDatasets(), pb_);

    for_i (reflCount)
      calcPoints_.append(hub_.makeReflectionInfos(
          *reflections.at(i), gammaSlices, rgeGamma, &progress));
  }

  interpolate();
}

void Frame::interpolate() {
  TakesLongTime __;

  interpPoints_.clear();

  auto pi = params_->panelInterpolation;
  if (pi) {
    typ::deg alphaStep   = pi->stepAlpha->value();
    typ::deg betaStep    = pi->stepBeta->value();
    qreal    idwRadius   = pi->idwRadius->value();

    qreal    avgRadius   = pi->avgRadius->value();
    qreal    avgAlphaMax = pi->avgAlphaMax->value();
    qreal    avgTreshold = pi->avgThreshold->value() / 100.0;

    for_i (calcPoints_.count())
      interpPoints_.append(
          calc::interpolate(calcPoints_.at(i),
                            alphaStep, betaStep, idwRadius,
                            avgAlphaMax, avgRadius, avgTreshold));
  } else {
    for_i (calcPoints_.count())
      interpPoints_.append(calc::ReflectionInfos());
  }

  displayReflection(getReflIndex(), getInterpolated());
}

void Frame::displayReflection(uint reflIndex, bool interpolated) {
  table_->clear();

  EXPECT(calcPoints_.count() == interpPoints_.count())
  if (calcPoints_.count() <= reflIndex)
    return;

  for (auto& r : (interpolated ? interpPoints_ : calcPoints_).at(reflIndex))
    table_->addRow(r.data(), false);

  table_->sortData();
}

uint Frame::getReflIndex() const {
  EXPECT(params_->panelReflection)
  int reflIndex = params_->panelReflection->cbRefl->currentIndex();
  RUNTIME_CHECK(reflIndex >= 0, "invalid reflection index");
  return reflIndex;
}

bool Frame::getInterpolated() const {
  auto pi = params_->panelPoints;
  return pi ? pi->rbInterp->isChecked() : false;
}

void Frame::logMessage(rcstr msg) const {
  MessageLogger::info(msg);
}

void Frame::logSuccess(bool res) const {
  if (res)
    MessageLogger::popup("Saved.");
  else
    MessageLogger::popup("The output could not be saved.");
}

bool Frame::logCheckSuccess(rcstr path, bool res) const {
  if (!res)
    MessageLogger::popup(str("'%1' could not be saved.").arg(path));
  return res;
}

//------------------------------------------------------------------------------
}}
// eof
