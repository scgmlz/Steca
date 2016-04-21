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
#include "types/type_models.h"
#include "thehub.h"

#include <QAbstractTableModel>
#include <QHeaderView>
#include <QFontMetrics>
#include <QSplitter>
#include <QScrollArea>
#include <QDate>

namespace gui { namespace io {
//------------------------------------------------------------------------------

class OutTableModel: public models::TableModel {
  SUPER(OutTableModel,models::TableModel)

public:
  OutTableModel(TheHub&, uint numColumns);

  int columnCount(rcIndex = models::ANY_INDEX) const;
  int rowCount(rcIndex    = models::ANY_INDEX) const;

  QVariant data(rcIndex,int) const;
  QVariant headerData(int, Qt::Orientation, int)  const;

  void moveColumn(uint from, uint to);

  void setHeaders(str_lst const&);
  void setCmpFuns(OutTable::cmp_vec const&);

  void clear();
  void addRow(OutTable::row_t const&);

  void sortData();

private:
  uint numColumns;

  uint_vec            colIndexMap;
  OutTable::cmp_vec   cmpFunctions;

  QVector<OutTable::row_t> printDatasets;
  str_lst columnTitles;
};

//------------------------------------------------------------------------------

OutTableModel::OutTableModel(TheHub& hub, uint numColumns_)
: models::TableModel(hub), numColumns(numColumns_) {
  colIndexMap.resize(numColumns);
  for_i (numColumns)
    colIndexMap[i] = i;
}

int OutTableModel::columnCount(rcIndex) const {
  return numColumns + 1;
}

int OutTableModel::rowCount(rcIndex) const {
  return printDatasets.count();
}

// The first column contains row numbers. The rest numCols columns contain data.

QVariant OutTableModel::data(rcIndex index, int role) const {
  int indexRow = index.row(), indexCol = index.column();
  int numRows = rowCount(), numCols = columnCount();

  if (indexCol < 0 || indexRow < 0)
    return QVariant();

  switch (role) {
  case Qt::DisplayRole:
    if (0 == indexCol)
      return indexRow + 1;

    if (--indexCol < numCols && indexRow < numRows)
      return printDatasets[indexRow][indexCol];

    break;

  case Qt::TextAlignmentRole:
    if (0 == indexCol)
      return Qt::AlignRight;

    if (--indexCol < numCols && indexRow < numRows) {
      switch (printDatasets[indexRow][indexCol].type()) {
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

QVariant OutTableModel::headerData(int section, Qt::Orientation, int role) const {
  if (section < 0 || columnTitles.count() < section)
    return QVariant();

  if (Qt::DisplayRole == role) {
    return 0 == section
        ? "#"
        : columnTitles.at(section - 1);
  }

  return QVariant();
}

void OutTableModel::moveColumn(uint from, uint to) {
  ASSERT(from < (uint)colIndexMap.count() && to < (uint)colIndexMap.count())

  qSwap(colIndexMap[from],colIndexMap[to]);
  sortData();
}

void OutTableModel::setCmpFuns(OutTable::cmp_vec const& cmps) {
  ASSERT(cmps.count() == (int)numColumns)
  cmpFunctions = cmps;
}

void OutTableModel::clear() {
  beginResetModel();
  printDatasets.clear();
  endResetModel();
}

void OutTableModel::setHeaders(str_lst const& headers) {
  ASSERT(headers.count() == (int)numColumns)
  columnTitles = headers;
}

void OutTableModel::addRow(OutTable::row_t const& row) {
  printDatasets.append(row);
  sortData();
}

void OutTableModel::sortData() {
  // sort left-to-right
  auto cmp = [this](OutTable::row_t const& r1, OutTable::row_t const& r2) {
    for_i (numColumns) {
      int c = cmpFunctions[colIndexMap[i]](r1[colIndexMap[i]], r2[colIndexMap[i]]);
      if (c < 0) return true;
      if (c > 0) return false;
    }
    return false;
  };

  beginResetModel();
  std::sort(printDatasets.begin(),printDatasets.end(),cmp);
  endResetModel();
}

//------------------------------------------------------------------------------

#define IMPL_CMP(name, toType)                             \
  int name(QVariant const& v1, QVariant const& v2) { \
    auto val1 = v1.toType(), val2 = v2.toType();           \
    if (val1 < val2) return -1;                            \
    if (val1 > val2) return +1;                            \
    return 0;                                              \
  }

IMPL_CMP(cmp_int,  toInt)
IMPL_CMP(cmp_str,  toString)
IMPL_CMP(cmp_real, toDouble)
IMPL_CMP(cmp_date, toDate)

//------------------------------------------------------------------------------

class OutTableHeaderView: public QHeaderView {
  SUPER(OutTableHeaderView,QHeaderView)
public:
  OutTableHeaderView();
};

OutTableHeaderView::OutTableHeaderView(): super(Qt::Horizontal) {
  setSectionsMovable(true);
}

//------------------------------------------------------------------------------

OutTable::OutTable(TheHub& hub, uint numDataColumns) {
  setHeader(new OutTableHeaderView);
  setModel((model_ = new OutTableModel(hub,numDataColumns)));

  header()->setSectionResizeMode(0,QHeaderView::Fixed);

  int w = QFontMetrics(header()->font()).width("000000000");
  setColumnWidth(0,w);
}

void OutTable::setHeaders(str_lst const& headers) {
  model_->setHeaders(headers);

  connect(header(),&QHeaderView::sectionMoved, [this](int /*logicalIndex*/, int oldVisualIndex, int newVisualIndex) {
    ASSERT(oldVisualIndex>0 && newVisualIndex>0)
    model_->moveColumn((uint)(oldVisualIndex-1),(uint)(newVisualIndex-1));
  });
}

void OutTable::setCmpFuns(OutTable::cmp_vec const& cmps) {
  model_->setCmpFuns(cmps);
}

void OutTable::clear() {
  model_->clear();
}

void OutTable::addRow(OutTable::row_t const& row) {
  model_->addRow(row);
}

//------------------------------------------------------------------------------

OutTableWidget::OutTableWidget(TheHub& hub,
                               str_lst const& headers, OutTable::cmp_vec const& cmps)
{
  ASSERT(headers.count() == cmps.count())
  uint numDataColumns = headers.count();

  QBoxLayout *box;
  setLayout((box = vbox()));
  box->setMargin(0);

  auto split = new QSplitter(Qt::Horizontal);
  box->addWidget(split);

  split->setChildrenCollapsible(false);
  split->addWidget((outTable_ = new OutTable(hub,numDataColumns)));

  outTable_->setHeaders(headers);
  outTable_->setCmpFuns(cmps);

  for_i (numDataColumns) {
    ShowColumn item; item.name = headers[i];
    showColumns_.append(item);
  }

  auto scrollArea = new QScrollArea;
  scrollArea->setWidget((showColumnsWidget_ = new ShowColumnsWidget(showColumns_)));

  for_i (numDataColumns) {
    auto cb = showColumns_[i].cb;

    cb->setChecked(true);
    connect(cb, &QCheckBox::clicked,[this,cb,i]() {
      if (cb->isChecked())
        outTable_->showColumn(i+1);
      else
        outTable_->hideColumn(i+1);
      }
    );
  }

  split->addWidget(scrollArea);
  split->setStretchFactor(0,1);
}

OutTableWidget::~OutTableWidget () {
}

//------------------------------------------------------------------------------

OutTableWidget::ShowColumnsWidget::ShowColumnsWidget(OutTableWidget::showcolumn_vec& items) {
  setLayout((grid_ = gridLayout()));

  for_i (items.count()) {
    auto &item = items[i];
    grid_->addWidget((item.cb = check(item.name)), i, 0);
  }
}

//------------------------------------------------------------------------------

OutWindow::OutWindow(rcstr title, QWidget* parent): super(parent, Qt::Dialog) {
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowTitle(title);
  setLayout((box_ = vbox()));
}

void OutWindow::setWidgets(panel::BasicPanel* p, QWidget* tw) {
  box_->addWidget((panel_       = p));
  box_->addWidget((widget_ = tw));
  auto bbox = hbox();
  box_->addLayout(bbox);
  box_->setStretch(1,1);

  auto actClose = new TriggerAction("Close", "Close", this);
  actClose->dialog();
  connect(actClose, &QAction::triggered, [this]() {
    close();
  });
  bbox->addWidget(textButton(actClose));

  bbox->addStretch();

  auto actCalculate = new TriggerAction("Calculate", "Calculate", this);
  actCalculate->dialog();
  connect(actCalculate, &QAction::triggered, [this]() {
    calculate();
  });
  bbox->addWidget(textButton(actCalculate));

  auto actSave = new TriggerAction("Save", "Save", this);
  actSave->dialog();
  bbox->addWidget(textButton(actSave));
}

//------------------------------------------------------------------------------
}}
// eof

