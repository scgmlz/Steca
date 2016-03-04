#include "fitting.h"
#include "thehub.h"
#include "core_fit_limits.h"
#include <QStyledItemDelegate>
#include <QAction>
#include <QApplication>

namespace panel {
//------------------------------------------------------------------------------

class ReflectionTypeDelegate: public QStyledItemDelegate {
  SUPER(ReflectionTypeDelegate,QStyledItemDelegate)
public:
  ReflectionTypeDelegate();

  QWidget *createEditor(QWidget*, QStyleOptionViewItem const&, QModelIndex const&)      const;
  void setEditorData(QWidget*, QModelIndex const&)                                      const;
  void setModelData(QWidget*, QAbstractItemModel*, QModelIndex const&)                  const;
  void updateEditorGeometry(QWidget*, QStyleOptionViewItem const&, QModelIndex const&)  const;
  void paint(QPainter*, QStyleOptionViewItem const&, QModelIndex const&)                const;

protected:
  str_lst items;
};

ReflectionTypeDelegate::ReflectionTypeDelegate() {
  items << "Integral" << "Gaussian" << "Lorentzian" << "PseudoVoigt1" << "PseudoVoigt2";
}

QWidget* ReflectionTypeDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const&, QModelIndex const&) const {
  auto editor = new QComboBox(parent);
  editor->addItems(items);
  return editor;
}

void ReflectionTypeDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const {
  auto comboBox = static_cast<QComboBox*>(editor);
  comboBox->setCurrentIndex(index.model()->data(index).toInt());
}

void ReflectionTypeDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const {
  auto comboBox = static_cast<QComboBox*>(editor);
  model->setData(index, comboBox->currentIndex());
}

void ReflectionTypeDelegate::updateEditorGeometry(QWidget* editor, QStyleOptionViewItem const& option, QModelIndex const&) const {
  editor->setGeometry(option.rect);
}

void ReflectionTypeDelegate::paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const {
  auto myOption = option;
  uint i = qBound(0, index.model()->data(index).toInt(), items.count()-1);
  myOption.text = items[i];
  QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOption, painter);
}

ReflectionView::ReflectionView(TheHub& theHub_)
: theHub(theHub_), model(theHub.reflectionViewModel) {

  setModel(&model);
  setItemDelegateForColumn(Model::COLUMN_TYPE, new ReflectionTypeDelegate);

  for_i (model.columnCount())
    resizeColumnToContents(i);
}

void ReflectionView::addReflection() {
  model.addReflection();
  update();
}

void ReflectionView::removeSelected() {
  auto index = currentIndex();
  if (!index.isValid()) return;

  int row = index.row();
  index = (row < model.rowCount()) ? index : index.sibling(row-1,0);
  model.remReflection(row);
  update();
}

void ReflectionView::update() {
  model.signalReset();
  auto index = currentIndex();
  // keep the current index, or select the last item
  setCurrentIndex(index.isValid()
    ? index
    : model.index(qMax(0,model.rowCount()-1),0));

  theHub.actReflectionRemove->setEnabled(model.rowCount() > 0);
}

void ReflectionView::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) {
  super::selectionChanged(selected,deselected);
//  auto indexes = selected.indexes();
//  model.setSelectedDataset(indexes.isEmpty()
//    ? core::shp_Dataset()
//    : model.data(indexes.first(), Model::GetDatasetRole).value<core::shp_Dataset>());
}

//------------------------------------------------------------------------------

Fitting::Fitting(TheHub& theHub_)
: super(EMPTY_STR,theHub_,Qt::Vertical) {

  box->addWidget(label("Background polynomial"));
  auto hb = hbox();
  box->addLayout(hb);

  hb->addWidget(iconButton(theHub.actBackgroundBackground));
  hb->addWidget(iconButton(theHub.actBackgroundClear));
  hb->addSpacing(65);
  hb->addWidget(label("Degree:"));
  hb->addWidget((spinDegree = spinCell(4,0,core::fit::MAX_BACKGROUND_POLYNOMIAL_DEGREE)));
  hb->addWidget(iconButton(theHub.actBackgroundShowFit));
  hb->addStretch();

  connect(spinDegree, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int degree) {
    theHub.setBackgroundPolynomialDegree(degree);
  });

  box->addWidget(label("Reflections"));
  auto hs = hbox();
  box->addLayout(hs);

  hs->addWidget(iconButton(theHub.actSelectPeak));
  hs->addWidget(iconButton(theHub.actReflectionPeak));
  hs->addWidget(iconButton(theHub.actReflectionWidth));
  hs->addStretch();

  box->addWidget((reflectionView = new ReflectionView(theHub)));

  auto hr = hbox();
  box->addLayout(hr);
  hr->addStretch();
  hr->addWidget(iconButton(theHub.actReflectionAdd));
  hr->addWidget(iconButton(theHub.actReflectionRemove));

  auto vb = vbox();
  box->addLayout(vb);
  vb->addWidget(label("info1"));
  vb->addWidget(label("info2"));
  vb->addWidget(label("info3"));

  connect(theHub.actReflectionAdd, &QAction::triggered, [this]() {
    reflectionView->addReflection();
  });

  connect(theHub.actReflectionRemove, &QAction::triggered, [this]() {
    reflectionView->removeSelected();
  });
}

//------------------------------------------------------------------------------
}
// eof
