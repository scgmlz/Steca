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

#include <QAbstractTableModel>
#include <QHeaderView>
#include <QFontMetrics>
#include <QSplitter>
#include <QScrollArea>
#include <QDate>

//------------------------------------------------------------------------------

class OutTableModel: public QAbstractTableModel {
  SUPER(OutTableModel,QAbstractTableModel)

public:
  OutTableModel(uint numColumns);

  int columnCount(QModelIndex const&)             const;
  int rowCount(QModelIndex const&)                const;
  QVariant data(QModelIndex const&,int)           const;
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

OutTableModel::OutTableModel(uint numColumns_): numColumns(numColumns_) {
  colIndexMap.resize(numColumns);
  for_i (numColumns)
    colIndexMap[i] = i;
}

int OutTableModel::columnCount(QModelIndex const&) const {
  return numColumns + 1;
}

int OutTableModel::rowCount(QModelIndex const&) const {
  return printDatasets.count();
}

// The first column contains row numbers. The rest numCols columns contain data.

QVariant OutTableModel::data(QModelIndex const& index, int role) const {
  int indexRow = index.row(), indexCol = index.column();
  int numRows  = rowCount(QModelIndex()), numCols   = columnCount(QModelIndex());

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

OutTable::OutTable(uint numDataColumns) {
  setHeader(new OutTableHeaderView);
  setModel((model = new OutTableModel(numDataColumns)));

  header()->setSectionResizeMode(0,QHeaderView::Fixed);

  // TODO if the collapse marker in the first column can be hidden,
  // or a delegate used, the width can be narrower
  int w = QFontMetrics(header()->font()).width("000000000");
  setColumnWidth(0,w);
}

void OutTable::setHeaders(str_lst const& headers) {
  model->setHeaders(headers);

  connect(header(),&QHeaderView::sectionMoved, [this](int /*logicalIndex*/, int oldVisualIndex, int newVisualIndex) {
    ASSERT(oldVisualIndex>0 && newVisualIndex>0)
    model->moveColumn((uint)(oldVisualIndex-1),(uint)(newVisualIndex-1));
  });
}

void OutTable::setCmpFuns(OutTable::cmp_vec const& cmps) {
  model->setCmpFuns(cmps);
}

void OutTable::clear() {
  model->clear();
}

void OutTable::addRow(OutTable::row_t const& row) {
  model->addRow(row);
}

//------------------------------------------------------------------------------

OutTableWidget::OutTableWidget(TheHub& theHub_,
                               str_lst const& headers, OutTable::cmp_vec const& cmps)
: theHub(theHub_) {

  ASSERT(headers.count() == cmps.count())
  uint numDataColumns = headers.count();

  QBoxLayout *box;
  setLayout((box = vbox()));

  auto split = new QSplitter(Qt::Horizontal);
  box->addWidget(split);

  split->setChildrenCollapsible(false);
  split->addWidget((outTable = new OutTable(numDataColumns)));

  outTable->setHeaders(headers);
  outTable->setCmpFuns(cmps);

  for_i (numDataColumns) {
    ShowItem item; item.tag = headers[i];
    showItems.append(item);
  }

  auto scrollArea = new QScrollArea;
  scrollArea->setWidget((showItemsWidget = new ShowItemsWidget(showItems)));

  for_i (numDataColumns) {
    auto cb = showItems[i].cb;

    cb->setChecked(true);
    connect(cb, &QCheckBox::clicked,[this,cb,i]() {
      if (cb->isChecked())
        outTable->showColumn(i+1);
      else
        outTable->hideColumn(i+1);
      }
    );
  }

  split->addWidget(scrollArea);
  split->setStretchFactor(0,1);
}

OutTableWidget::~OutTableWidget () {
}

//------------------------------------------------------------------------------

OutTableWidget::ShowItemsWidget::ShowItemsWidget(OutTableWidget::showitem_vec& items) {
  setLayout((grid = gridLayout()));

  for_i (items.count()) {
    auto &item = items[i];
    grid->addWidget((item.cb = check(item.tag)), i, 0);
  }
}

//------------------------------------------------------------------------------

OutWindow::OutWindow(rcstr title, QWidget* parent): super(parent, Qt::Dialog) {
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowTitle(title);
  setLayout((box = vbox()));
}

void OutWindow::setWidgets(panel::BasicPanel* p, OutTableWidget* tw) {
  box->addWidget((panel       = p));
  box->addWidget((tableWidget = tw));
  connect(tableWidget, &OutTableWidget::calculate, this, &thisClass::calculate);

  auto bbox = hbox();
  box->addLayout(bbox);

  auto actClose = new PushAction("Close", "Close...", "", this);
  connect(actClose, &QAction::triggered, [this]() {
    close();
  });
  bbox->addWidget(textButton(actClose));

  bbox->addStretch();

  auto actCalculate = new PushAction("Calculate", "Calculate...", "", this);
  connect(actCalculate, &QAction::triggered, [this]() {
    emit calculate();
  });
  bbox->addWidget(textButton(actCalculate));

  auto actSave = new PushAction("Save...", "Save...", "", this);
  bbox->addWidget(textButton(actSave));
}

//------------------------------------------------------------------------------
// eof

