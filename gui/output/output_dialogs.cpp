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
#include "core_polefigure.h"
#include "thehub.h"
#include "types/core_async.h"
#include "types/type_models.h"

#include <QFileDialog>
#include <QHeaderView>
#include <QProgressBar>
#include <QScrollArea>
#include <QSplitter>
#include <QPushButton>

namespace gui { namespace output {
//------------------------------------------------------------------------------

static str const GROUP_OUT_PARAMS("Output Params");
static str const
  KEY_GAMMA_STEP("gamma step"),
  KEY_GAMMA_MIN("gamma min"),
  KEY_GAMMA_MAX("gamma max"),
  KEY_ALPHA_STEP("alpha step"),
  KEY_BETA_STEP("beta step"),
  KEY_AVG_ALPHA_MAX("avg alpha max"),
  KEY_AVG_RADIUS("avg radius"),
  KEY_IDW_RADIUS("idw radius"),
  KEY_THRESHOLD("threshold");

Params::Params(TheHub& hub) : RefHub(hub) {
  setLayout((box_ = boxLayout(Qt::Horizontal)));
  box_->setMargin(0);

  {
    box_->addWidget((gpRefl_ = new panel::GridPanel(hub, "Reflection")));
    auto g = gpRefl_->grid();

    g->addWidget((cbRefl = comboBox(hub_.reflectionsModel.names())), 0, 0, 1, 2);

    g->addWidget((rbCalc   = radioButton("calc.")),   1, 0);
    g->addWidget((rbInterp = radioButton("interp.")), 1, 1);

    g->setRowStretch(g->rowCount(), 1);
  }

  {
    box_->addWidget((gpGamma_ = new panel::GridPanel(hub, "Gamma")));
    auto g = gpGamma_->grid();

    g->addWidget(label("step"), 0, 0, Qt::AlignRight);
    g->addWidget((stepGamma = spinCell(6, 1., 30.)), 0, 1);

    g->addWidget((cbLimitGamma = check("limit range")), 0, 2, 1, 2);

    g->addWidget(label("min"), 1, 0, Qt::AlignRight);
    g->addWidget((limitGammaMin = spinCell(6, -180., 180.)), 1, 1);

    g->addWidget(label("max"), 1, 2, Qt::AlignRight);
    g->addWidget((limitGammaMax = spinCell(6, -180., 180.)), 1, 3);

    g->setRowStretch(g->rowCount(), 1);
  }

  {
    box_->addWidget((gpInterpolation_ = new panel::GridPanel(hub, "Interpolation")));
    auto g = gpInterpolation_->grid();

    g->addWidget(label("step α"), 0, 0, Qt::AlignRight);
    g->addWidget((stepAlpha = spinCell(6, 1., 30.)), 0, 1);
    g->addWidget(label("β"), 1, 0, Qt::AlignRight);
    g->addWidget((stepBeta = spinCell(6, 1., 30.)), 1, 1);
    g->addWidget(label("r. idw"), 2, 0, Qt::AlignRight);
    g->addWidget((idwRadius = spinCell(6, 0., 90.)), 2, 1);

    g->addWidget(label("avg α max"), 0, 2, Qt::AlignRight);
    g->addWidget((avgAlphaMax = spinCell(6, 0., 90.)), 0, 3);
    g->addWidget(label("r. avg"), 1, 2, Qt::AlignRight);
    g->addWidget((avgRadius = spinCell(6, 0., 90.)), 1, 3);
    g->addWidget(label("incl. %"), 2, 2, Qt::AlignRight);
    g->addWidget((threshold = spinCell(6, 0, 100)), 2, 3);

    g->setRowStretch(g->rowCount(), 1);
  }

  Settings s(GROUP_OUT_PARAMS);

  stepGamma->setValue(s.readReal(KEY_GAMMA_STEP, 5));
  limitGammaMin->setValue(s.readReal(KEY_GAMMA_MIN, 0));
  limitGammaMax->setValue(s.readReal(KEY_GAMMA_MAX, 0));

  stepAlpha->setValue(s.readReal(KEY_ALPHA_STEP, 5));
  stepBeta->setValue(s.readReal(KEY_BETA_STEP, 5));

  avgAlphaMax->setValue(s.readReal(KEY_AVG_ALPHA_MAX, 15));
  avgRadius->setValue(s.readReal(KEY_AVG_RADIUS, 5));
  idwRadius->setValue(s.readReal(KEY_IDW_RADIUS, 10));
  threshold->setValue(s.readReal(KEY_THRESHOLD, 100));

  auto enableLimitGamma = [this](bool on) {
    cbLimitGamma->setChecked(on);
    limitGammaMin->setEnabled(on);
    limitGammaMax->setEnabled(on);
  };

  connect(cbLimitGamma, &QCheckBox::toggled, [enableLimitGamma](int on) {
    enableLimitGamma(on);
  });

  enableLimitGamma(false);
  rbCalc->click();
}

Params::~Params() {
  Settings s(GROUP_OUT_PARAMS);

  s.saveReal(KEY_GAMMA_STEP, stepGamma->value());
  s.saveReal(KEY_GAMMA_MIN,  limitGammaMin->value());
  s.saveReal(KEY_GAMMA_MAX,  limitGammaMax->value());

  s.saveReal(KEY_ALPHA_STEP, stepAlpha->value());
  s.saveReal(KEY_BETA_STEP,  stepBeta->value());

  s.saveReal(KEY_AVG_ALPHA_MAX, avgAlphaMax->value());
  s.saveReal(KEY_AVG_RADIUS, avgRadius->value());
  s.saveReal(KEY_IDW_RADIUS, idwRadius->value());
  s.saveReal(KEY_THRESHOLD,  threshold->value());
}

void Params::addStretch() {
  box_->addStretch();
}

int Params::currReflIndex() const {
  return cbRefl->currentIndex();
}

bool Params::interpolate() const {
  return rbInterp->isChecked();
}

//------------------------------------------------------------------------------

Tabs::Tabs(TheHub& hub) : super(hub) {
}

//------------------------------------------------------------------------------

Tab::Tab(TheHub& hub, Params& params)
: RefHub(hub), params_(params) {
  setLayout((grid_ = gridLayout()));
}

//------------------------------------------------------------------------------

Frame::Frame(TheHub& hub, rcstr title, Params* params, QWidget* parent)
: super(parent, Qt::Dialog), RefHub(hub)
{
  setAttribute(Qt::WA_DeleteOnClose);
  auto flags = windowFlags();
  setWindowFlags(flags & ~Qt::WindowContextHelpButtonHint);
  setWindowTitle(title);
  setLayout((box_ = vbox()));

  EXPECT(params)
  params->addStretch();

  box_->addWidget((params_ = params));
  box_->addWidget((tabs_   = new Tabs(hub)));
  box_->setStretch(1, 1);

  auto bbox = hbox();
  box_->addLayout(bbox);

  actClose_       = new TriggerAction("Close",       this);
  actCalculate_   = new TriggerAction("Calculate",   this);
  actInterpolate_ = new TriggerAction("Interpolate", this);

  bbox->addWidget((btnClose_ = textButton(actClose_)));
  bbox->addStretch(1);
  bbox->addWidget((pb_ = new QProgressBar));
  bbox->addStretch(1);
  bbox->addWidget((btnCalculate_  = textButton(actCalculate_)));
  bbox->addWidget((btnInterpolate_= textButton(actInterpolate_)));

  bbox->setStretchFactor(pb_, 333);
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

  auto display = [this]() {
    displayReflection(params_->currReflIndex(), params_->interpolate());
  };

  connect(params_->cbRefl, slot(QComboBox,currentIndexChanged,int), [display]() {
    display();
  });

  connect(params_->rbInterp, &QRadioButton::toggled, [display]() {
    display();
  });

  // tabs

  auto tabTable = new TabTable(hub, *params_,
     core::ReflectionInfo::dataTags(), core::ReflectionInfo::dataCmps());
  tabs_->addTab("Points").box().addWidget(tabTable);

  table_ = tabTable->table();
}

void Frame::calculate() {
  TakesLongTime __;

  calcPoints_.clear();
  interpPoints_.clear();

  auto &reflections = hub_.reflections();
  if (!reflections.isEmpty()) {
    uint reflCount = reflections.count();

    core::deg gammaStep = params_->stepGamma->value();

    core::Range rgeGamma;
    if (params_->cbLimitGamma->isChecked())
      rgeGamma.safeSet(params_->limitGammaMin->value(),
                       params_->limitGammaMax->value());

    Progress progress(reflCount * hub_.numCollectedDatasets(), pb_);

    for_i (reflCount)
      calcPoints_.append(hub_.makeReflectionInfos(
          *reflections.at(i), gammaStep, rgeGamma, &progress));
  }

  interpolate();
}

void Frame::interpolate() {
  TakesLongTime __;

  core::deg alphaStep   = params_->stepAlpha->value();
  core::deg betaStep    = params_->stepBeta->value();
  qreal     avgRadius   = params_->avgRadius->value();
  qreal     avgAlphaMax = params_->avgAlphaMax->value();
  qreal     idwRadius   = params_->idwRadius->value();
  qreal     treshold    = params_->threshold->value() / 100.0;

  interpPoints_.clear();

  for_i (calcPoints_.count()) {
    interpPoints_.append(
        core::pole::interpolate(calcPoints_.at(i), alphaStep, betaStep,
                                avgAlphaMax, avgRadius, idwRadius, treshold));
  }

  displayReflection(params_->currReflIndex(), params_->interpolate());
}

void Frame::displayReflection(uint reflIndex, bool interpolated) {
  table_->clear();

  if (hub_.reflections().isEmpty())
    return;

  EXPECT(calcPoints_.count() == interpPoints_.count())
  if (calcPoints_.count() <= (int)reflIndex)
    return;

  for (auto const &r : (interpolated ? interpPoints_ : calcPoints_)[reflIndex])
    table_->addRow(r.data(), false);

  table_->sortData();
}

//------------------------------------------------------------------------------

class TableModel : public models::TableModel {
  SUPER(TableModel, models::TableModel)

public:
  TableModel(TheHub&, uint numCols_);

  int columnCount(rcIndex = models::ANY_INDEX) const;
  int rowCount(rcIndex = models::ANY_INDEX)    const;

  QVariant data(rcIndex, int) const;
  QVariant headerData(int, Qt::Orientation, int) const;

  void moveColumn(uint from, uint to);

  void setColumns(str_lst const& headers, core::cmp_vec const&);
  str_lst const headers() { return headers_; }

  void setSortColumn(int);

  void clear();
  void addRow(core::row_t const&, bool sort = true);

  core::row_t const& row(uint);

  void sortData();

private:
  uint numCols_;
  int  sortColumn_;

  str_lst       headers_;
  uint_vec      colIndexMap_;
  core::cmp_vec cmpFunctions_;

  QVector<core::row_t> rows_;
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
  return numCols_ + 1;
}

int TableModel::rowCount(rcIndex) const {
  return rows_.count();
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
      QVariant var = rows_.at(indexRow).at(indexCol);
      if (QVariant::Double == var.type() && qIsNaN(var.toDouble()))
        var = QVariant(); // hide nans
      return var;
    }

    break;

  case Qt::TextAlignmentRole:
    if (0 == indexCol)
      return Qt::AlignRight;

    if (--indexCol < numCols && indexRow < numRows) {
      switch (rows_.at(indexRow).at(indexCol).type()) {
      case QVariant::Int:
      case QVariant::UInt:
      case QVariant::LongLong:
      case QVariant::ULongLong:
      case QVariant::Double:
      case QVariant::Date:
        return Qt::AlignRight;
      default:
        break;
      }
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
    return 0 == section ? "#" : headers_.at(section - 1);

  return QVariant();
}

void TableModel::moveColumn(uint from, uint to) {
  EXPECT(from < (uint)colIndexMap_.count() && to < (uint)colIndexMap_.count())
  qSwap(colIndexMap_[from], colIndexMap_[to]);
}

void TableModel::setColumns(str_lst const& headers, core::cmp_vec const& cmps) {
  EXPECT(headers.count() == (int)numCols_ && cmps.count() == (int)numCols_)
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

void TableModel::addRow(core::row_t const& row, bool sort) {
  rows_.append(row);
  if (sort)
    sortData();
}

core::row_t const& TableModel::row(uint index) {
  return rows_.at(index);
}

void TableModel::sortData() {
  // sort sortColumn first, then left-to-right
  auto cmpRows = [this](uint i, core::row_t const& r1, core::row_t const& r2) {
    i = colIndexMap_.at(i);
    return cmpFunctions_.at(i)(r1.at(i), r2.at(i));
  };

  auto cmp = [this, cmpRows](core::row_t const& r1, core::row_t const& r2) {
    if (sortColumn_ >= 0) {
      int c = cmpRows(sortColumn_, r1, r2);
      if (c < 0) return true;
      if (c > 0) return false;
    }

    for_i (numCols_) {
      if (i != sortColumn_) {
        int c = cmpRows(i, r1, r2);
        if (c < 0) return true;
        if (c > 0) return false;
      }
    }

    return false;
  };

  beginResetModel();
  std::sort(rows_.begin(), rows_.end(), cmp);
  endResetModel();
}

//------------------------------------------------------------------------------

Table::Table(TheHub& hub, uint numDataColumns)
: RefHub(hub), model_(nullptr)
{
  model_.reset(new TableModel(hub_, numDataColumns));
  setModel(model_.data());
  setHeader(new QHeaderView(Qt::Horizontal));

  auto& h = *header();

  h.setSectionResizeMode(0, QHeaderView::Fixed);
  h.setSectionsMovable(true);
  h.setSectionsClickable(true);

  int w = QFontMetrics(h.font()).width("000000000");
  setColumnWidth(0, w);
}

void Table::setColumns(str_lst const& headers, core::cmp_vec const& cmps) {
  model_->setColumns(headers, cmps);

  connect(header(), &QHeaderView::sectionMoved,
          [this](int /*logicalIndex*/, int oldVisualIndex, int newVisualIndex) {
            ENSURE(oldVisualIndex > 0 && newVisualIndex > 0)
            auto& h = *header();
            h.setSortIndicatorShown(false);
            model_->setSortColumn(-1);
            model_->moveColumn((uint)(oldVisualIndex - 1),
                               (uint)(newVisualIndex - 1));
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

void Table::addRow(core::row_t const& row, bool sort) {
  model_->addRow(row, sort);
}

void Table::sortData() {
  model_->sortData();
}

uint Table::rowCount() const {
  return model_->rowCount();
}

const core::row_t&Table::row(uint i) const {
  return model_->row(i);
}

//------------------------------------------------------------------------------

TabTable::TabTable(TheHub& hub, Params& params,
                   str_lst const& headers, core::cmp_vec const& cmps)
: super(hub, params)
{
  EXPECT(headers.count() == cmps.count())
  uint numCols = headers.count();

  grid_->setMargin(0);
  grid_->addWidget((table_ = new Table(hub_, numCols)), 0, 0);
  grid_->setColumnStretch(0, 1);

  table_->setColumns(headers, cmps);

  for_i (numCols) {
    showcol_t item;
    item.name = headers.at(i);
    showCols_.append(item);
  }

  auto scrollArea = new QScrollArea;
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollArea->setWidget(
      (showColumnsWidget_ = new ShowColsWidget(*table_, showCols_)));

  grid_->addWidget(scrollArea, 0, 1);
}

//------------------------------------------------------------------------------
static uint PRESET_SELECTION = 3;
TabTable::ShowColsWidget::ShowColsWidget(Table& table, showcol_vec& showCols)
: table_(table), showCols_(showCols)
{
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
    showCols_.at((uint)eReflAttr::ALPHA).cb->setChecked(true);
    showCols_.at((uint)eReflAttr::BETA).cb->setChecked(true);
  };

  auto showInten = [this,showAlphaBeta]() {
    showAlphaBeta();
    showCols_.at((uint)eReflAttr::INTEN).cb->setChecked(true);
  };

  auto showTth = [this,showAlphaBeta]() {
    showAlphaBeta();
    showCols_.at((uint)eReflAttr::TTH).cb->setChecked(true);
  };

  auto showFWHM = [this,showAlphaBeta]() {
    showAlphaBeta();
    showCols_.at((uint)eReflAttr::FWHM).cb->setChecked(true);
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

      switch ((eReflAttr)i) {
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
    rbInten_->setChecked(!isOther && PRESET_SELECTION == nInten);
    rbTth_->setChecked(!isOther && PRESET_SELECTION == nTth);
    rbFWHM_->setChecked(!isOther && PRESET_SELECTION == nFwhm);
  };

  for_i (showCols_.count()) {
    auto cb = showCols_.at(i).cb;

    connect(cb, &QCheckBox::toggled, [this, i, updateRbs](bool on) {
      if (on)
        table_.showColumn(i + 1);
      else
        table_.hideColumn(i + 1);

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

TabSave::TabSave(TheHub& hub, Params& params)
: super(hub, params)
{
  path_ = new QLineEdit;
  path_->setReadOnly(true);
  path_->setText(QDir::current().canonicalPath()); // TODO Settings

  fileName_   = new QLineEdit;

  actBrowsePath_ = new TriggerAction("Browse...", this);
  actSave_       = new TriggerAction("Save", this);

  auto gp = new panel::GridPanel(hub, "Destination");
  grid_->addWidget(gp, 0, 0);

  auto g = gp->grid();

  g->addWidget(label("Save to folder:"),   0, 0, Qt::AlignRight);
  g->addWidget(path_,                      0, 1);
  g->addWidget(textButton(actBrowsePath_), 0, 2);

  g->addWidget(label("File name:"),  1, 0, Qt::AlignRight);
  g->addWidget(fileName_,            1, 1);
  g->addWidget(textButton(actSave_), 2, 1);

  g->setRowStretch(g->rowCount(), 1);

  filesSavedDialog_ = new QMessageBox(this);

  connect(actBrowsePath_, &QAction::triggered, [this]() {
    str dir = QFileDialog::getExistingDirectory(this, "Select folder", path_->text());
    if (!dir.isEmpty())
      path_->setText(dir);
  });
}

void TabSave::clearFilename() {
  fileName_->clear();
}

void TabSave::showMessage() {
  filesSavedDialog_->show();
}

void TabSave::savedMessage(str message) {
  filesSavedDialog_->setText(filesSavedDialog_->text() + message);
}

void TabSave::clearMessage() {
  filesSavedDialog_->setText("");
}


//------------------------------------------------------------------------------
}}
// eof
