// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      out_table.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "out_table.h"
#include "thehub.h"
#include "types/core_async.h"
#include "types/type_models.h"

#include <QAbstractTableModel>
#include <QFontMetrics>
#include <QHeaderView>
#include <QScrollArea>
#include <QSplitter>

namespace gui { namespace io {
//------------------------------------------------------------------------------

class OutTableModel : public models::TableModel {
  SUPER(OutTableModel, models::TableModel)

public:
  OutTableModel(TheHub&, uint numCols_);

  int columnCount(rcIndex = models::ANY_INDEX) const;
  int rowCount(rcIndex = models::ANY_INDEX) const;

  QVariant data(rcIndex, int) const;
  QVariant headerData(int, Qt::Orientation, int) const;

  void moveColumn(uint from, uint to);

  void setHeaders(str_lst const&);
  void setCmpFuns(core::cmp_vec const&);
  void setSortColumn(int);

  void clear();
  void addRow(core::row_t const&, bool sort = true);
  void addRow(core::row_t const&);
  core::row_t const& row(int);
  str_lst const colTitles() { return colTitles_; }

  void sortData();

private:
  uint numCols_;
  int  sortColumn_;

  str_lst       colTitles_;
  uint_vec      colIndexMap_;
  core::cmp_vec cmpFunctions_;

  QVector<core::row_t> rows_;
};

//------------------------------------------------------------------------------

OutTableModel::OutTableModel(TheHub& hub, uint numColumns_)
: models::TableModel(hub), numCols_(numColumns_), sortColumn_(0)
{
  colIndexMap_.resize(numCols_);
  for_i (numCols_)
    colIndexMap_[i] = i;
}

int OutTableModel::columnCount(rcIndex) const {
  return numCols_ + 1;
}

int OutTableModel::rowCount(rcIndex) const {
  return rows_.count();
}

// The first column contains row numbers. The rest numCols columns contain data.

QVariant OutTableModel::data(rcIndex index, int role) const {
  int indexRow = index.row(), indexCol = index.column();
  int numRows = rowCount(), numCols = columnCount();

  if (indexCol < 0 || indexRow < 0) return QVariant();

  switch (role) {
  case Qt::DisplayRole:
    if (0 == indexCol) return indexRow + 1;

    if (--indexCol < numCols && indexRow < numRows)
      return rows_[indexRow][indexCol];

    break;

  case Qt::TextAlignmentRole:
    if (0 == indexCol) return Qt::AlignRight;

    if (--indexCol < numCols && indexRow < numRows) {
      switch (rows_[indexRow][indexCol].type()) {
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

QVariant OutTableModel::headerData(int section, Qt::Orientation,
                                   int role) const {
  if (section < 0 || colTitles_.count() < section) return QVariant();

  if (Qt::DisplayRole == role) {
    return 0 == section ? "#" : colTitles_.at(section - 1);
  }

  return QVariant();
}

void OutTableModel::moveColumn(uint from, uint to) {
  EXPECT(from < (uint)colIndexMap_.count() && to < (uint)colIndexMap_.count())
  qSwap(colIndexMap_[from], colIndexMap_[to]);
}

void OutTableModel::setHeaders(str_lst const& headers) {
  EXPECT(headers.count() == (int)numCols_)
  colTitles_ = headers;
}

void OutTableModel::setCmpFuns(core::cmp_vec const& cmps) {
  EXPECT(cmps.count() == (int)numCols_)
  cmpFunctions_ = cmps;
}

void OutTableModel::setSortColumn(int col) {
  sortColumn_ = col;
}

void OutTableModel::clear() {
  beginResetModel();
  rows_.clear();
  endResetModel();
}

void OutTableModel::addRow(core::row_t const& row, bool sort) {
  rows_.append(row);
  if (sort)
    sortData();
}

core::row_t const& OutTableModel::row(int index) {
  return rows_.at(index);
}

void OutTableModel::sortData() {
  // sort sortColumn first, then left-to-right
  auto cmpRows = [this](uint i, core::row_t const& r1, core::row_t const& r2) {
    i = colIndexMap_[i];
    return cmpFunctions_[i](r1[i], r2[i]);
  };

  auto cmp = [this, cmpRows](core::row_t const& r1, core::row_t const& r2) {
    if (sortColumn_ > 0) {
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

OutTable::OutTable(TheHub& hub, uint numDataColumns) : model_(nullptr) {
  setModel((model_ = new OutTableModel(hub, numDataColumns)));
  setHeader(new QHeaderView(Qt::Horizontal));

  auto& h = *header();

  h.setSectionResizeMode(0, QHeaderView::Fixed);
  h.setSectionsMovable(true);
  h.setSectionsClickable(true);

  int w = QFontMetrics(h.font()).width("000000000");
  setColumnWidth(0, w);
}

OutTable::~OutTable() {
  delete model_;
}

void OutTable::setHeaders(str_lst const& headers) {
  model_->setHeaders(headers);

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

void OutTable::setCmpFuns(core::cmp_vec const& cmps) {
  model_->setCmpFuns(cmps);
}

void OutTable::clear() {
  model_->clear();
}

void OutTable::addRow(core::row_t const& row, bool sort) {
  model_->addRow(row, sort);
}

const core::row_t& OutTable::row(int index) {
  return model_->row(index);
}

const str_lst OutTable::colTitles() {
  return model_->colTitles();
}

void OutTable::sortData() {
  model_->sortData();
}

//------------------------------------------------------------------------------

OutTableWidget::OutTableWidget(TheHub& hub, str_lst const& headers,
                               core::cmp_vec const& cmps) {
  EXPECT(headers.count() == cmps.count())
  uint numDataColumns = headers.count();

  QBoxLayout* box;
  setLayout((box = vbox()));
  box->setMargin(0);

  auto split = new QSplitter(Qt::Horizontal);
  box->addWidget(split);

  split->setChildrenCollapsible(false);
  split->addWidget((outTable_ = new OutTable(hub, numDataColumns)));

  outTable_->setHeaders(headers);
  outTable_->setCmpFuns(cmps);

  for_i (numDataColumns) {
    ShowColumn item;
    item.name = headers[i];
    showColumns_.append(item);
  }

  auto scrollArea = new QScrollArea;
  scrollArea->setWidget(
      (showColumnsWidget_ = new ShowColumnsWidget(showColumns_)));

  for_i (numDataColumns) {
    auto cb = showColumns_[i].cb;

    cb->setChecked(true);
    connect(cb, &QCheckBox::toggled, [this, cb, i](bool on) {
      if (on)
        outTable_->showColumn(i + 1);
      else
        outTable_->hideColumn(i + 1);
    });
  }

  split->addWidget(scrollArea);
  split->setStretchFactor(0, 1);
}

void OutTableWidget::presetAll() {
  for (auto& col: showColumns_)
    col.cb->setChecked(true);
}

void OutTableWidget::presetNone() {
  for (auto& col: showColumns_)
    col.cb->setChecked(false);
}

//------------------------------------------------------------------------------

OutTableWidget::ShowColumnsWidget::ShowColumnsWidget(
    OutTableWidget::showcolumn_vec& items) {
  setLayout((grid_ = gridLayout()));

  for_i (items.count()) {
    auto& item = items[i];
    grid_->addWidget((item.cb = check(item.name)), i, 0);
  }
}

//------------------------------------------------------------------------------

SaveOutputWidget::SaveOutputWidget() {
  setLayout((box_ = vbox()));
  subGl_ = gridLayout();
  box_->addLayout(subGl_);

  auto gl = gridLayout();
  gl->setColumnMinimumWidth(0,60);
  gl->setColumnMinimumWidth(1,200);
  gl->setColumnMinimumWidth(2,80);
  gl->setColumnMinimumWidth(3,60);
  gl->setColumnMinimumWidth(4,300);
  gl->addWidget(label("Save in "));
  gl->addWidget(dirPath_ = readCell(40),0,1);
  browsePath_ = new TriggerAction("Browse...","Set directory were to save output",this);
  gl->addWidget(textButton(browsePath_),0,2);
  gl->addWidget(label("File name "),0,3);
  gl->addWidget(fileName_ = editCell(60),0,4);

  saveOutput_ = new TriggerAction("Save","Save files",this);
  gl->addWidget(textButton(saveOutput_),0,5);

  gl->setColumnStretch(6,1);
  gl->setRowStretch(1,1);
  box_->addLayout(gl);

  connect(browsePath_,&QAction::triggered,[this]() {
    QString dir = QFileDialog::getExistingDirectory(this, "Browse path", QDir::current().absolutePath());
    dirPath_->setText(dir);
  });

}

//------------------------------------------------------------------------------

OutWindow::OutWindow(TheHub& hub, rcstr title, QWidget* parent)
: super(parent, Qt::Dialog), RefHub(hub)
{
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowTitle(title);
  setLayout((box_ = vbox()));
}

void OutWindow::setWidgets(panel::BasicPanel* panel, QWidget* widget) {
  box_->addWidget((panel_ = panel));
  box_->addWidget(widget);
  auto bbox = hbox();
  box_->addLayout(bbox);
  box_->setStretch(1, 1);

  actClose_ = new TriggerAction("Close", "", this);
  connect(actClose_, &QAction::triggered, [this]() { close(); });
  bbox->addWidget(textButton(actClose_));

  bbox->addStretch();

  actCalculate_ = new TriggerAction("Calculate", "Calculate", this);
  connect(actCalculate_, &QAction::triggered, [this]() {
    TakesLongTime __;
    calculate();
  });

  bbox->addWidget(textButton(actCalculate_));
}

//------------------------------------------------------------------------------
}}
// eof
