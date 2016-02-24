#include "delegates.h"
#include <QPainter>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>
#include <QApplication>
#include <QMouseEvent>

//------------------------------------------------------------------------------

IconDelegate::IconDelegate(rcstr iconFile, bool showAlways_)
  : super(), icon(iconFile), showAlways(showAlways_) {
}

QSize IconDelegate::sizeHint(QStyleOptionViewItem const& option, QModelIndex const& index) const {
  QSize size = QStyledItemDelegate::sizeHint(option,index);

  if (!scaled) {
    icon = icon.scaledToHeight(size.height()-3,Qt::SmoothTransformation);
    scaled = true;
  }

  return size;
}

QPoint IconDelegate::iconPos(QStyleOptionViewItem const& option) const {
  return QPoint(option.rect.left(),
                option.rect.center().y() - icon.height()/2);
}

void IconDelegate::paint(QPainter *painter, QStyleOptionViewItem const& option, QModelIndex const& index) const {
  QStyledItemDelegate::paint(painter, option, index);
  if(showAlways || option.state & QStyle::State_MouseOver)
    painter->drawPixmap(iconPos(option), icon);
}

//------------------------------------------------------------------------------

CheckDelegate::CheckDelegate(bool springy_): springy(springy_) {
}

QWidget* CheckDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const&, QModelIndex const&) const {
    QCheckBox *cb = new QCheckBox(parent);
    return cb;
}

static QRect CheckBoxRect(QStyleOptionViewItem const& option) {
  QStyleOptionButton sob;
  QRect r = QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator, &sob);

  QRect const& sr = option.rect;
  QPoint p(sr.x() + sr.width()/2 - r.width()/2, sr.y() + sr.height()/2 - r.height()/2);

  return QRect(p, r.size());
}

bool CheckDelegate::editorEvent(QEvent* event, QAbstractItemModel* model,
    QStyleOptionViewItem const& option, QModelIndex const& index) {

    switch (event->type()) {
    case QEvent::MouseButtonRelease:
        if (!springy) return true;
        // fall through
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonDblClick: {
        QMouseEvent *me = static_cast<QMouseEvent*>(event);
        if (CheckBoxRect(option).contains(me->pos())) {
            bool val = model->data(index, Qt::CheckStateRole).toBool();
            model->setData(index, !val, Qt::CheckStateRole);
            return true;
        }
    }	break;
    default:
        break;
    }

    return super::editorEvent(event, model, option, index);
}

void CheckDelegate::paint(QPainter *painter,
    QStyleOptionViewItem const& option, QModelIndex const& index) const {

    bool val = index.model()->data(index, Qt::CheckStateRole).toBool();

    QStyleOptionButton sob;
    sob.state |= QStyle::State_Enabled;
    sob.state |= val ? QStyle::State_On : QStyle::State_Off;
    sob.rect = CheckBoxRect(option);

    QApplication::style()->drawControl(QStyle::CE_CheckBox, &sob, painter);
}

//------------------------------------------------------------------------------

RadioDelegate::RadioDelegate() {
}

QWidget* RadioDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const&, QModelIndex const&) const {
    return new QRadioButton(parent);
}

static QRect RadioBoxRect(QStyleOptionViewItem const& option) {
  QStyleOptionButton sob;
  QRect r = QApplication::style()->subElementRect(QStyle::SE_RadioButtonIndicator, &sob);

  QRect const& sr = option.rect;
  QPoint p(sr.x() + sr.width()/2 - r.width()/2, sr.y() + sr.height()/2 - r.height()/2);

  return QRect(p, r.size());
}

bool RadioDelegate::editorEvent(QEvent* event, QAbstractItemModel* model,
    QStyleOptionViewItem const& option, QModelIndex const& index) {

    switch (event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonDblClick: {
        QMouseEvent *me = static_cast<QMouseEvent*>(event);
        if (RadioBoxRect(option).contains(me->pos())) {
            bool val = model->data(index, Qt::CheckStateRole).toBool();
            model->setData(index, !val, Qt::CheckStateRole);
            return true;
        }
    }	break;
    default:
        break;
    }

    return super::editorEvent(event, model, option, index);
}

void RadioDelegate::paint(QPainter *painter,
    QStyleOptionViewItem const& option, QModelIndex const& index) const {

    bool val = index.model()->data(index, Qt::CheckStateRole).toBool();

    QStyleOptionButton sob;
    sob.state |= QStyle::State_Enabled;
    sob.state |= val ? QStyle::State_On : QStyle::State_Off;
    sob.rect = CheckBoxRect(option);

    QApplication::style()->drawControl(QStyle::CE_RadioButton, &sob, painter);
}

//------------------------------------------------------------------------------

ComboBoxDelegate::ComboBoxDelegate() {
  Items.push_back("Integral");
  Items.push_back("Lorentzian");
  Items.push_back("Gaussian");
}


QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const {
  QComboBox* editor = new QComboBox(parent);
  for(unsigned int i = 0; i < Items.size(); ++i)
    {
    editor->addItem(Items[i].c_str());
    }
  return editor;
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
  QComboBox *comboBox = static_cast<QComboBox*>(editor);
  int value = index.model()->data(index, Qt::EditRole).toUInt();
  comboBox->setCurrentIndex(value);
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
  QComboBox *comboBox = static_cast<QComboBox*>(editor);
  model->setData(index, comboBox->currentIndex(), Qt::EditRole);
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const {
  editor->setGeometry(option.rect);
}

void ComboBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
  QStyleOptionViewItemV4 myOption = option;
  QString text = Items[index.row()].c_str();

  myOption.text = text;

  QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOption, painter);
}

/*
//------------------------------------------------------------------------------

static bool disableDelegate(QModelIndex const& index) {
    return index.model()->data(index, QvDisableDelegateRole).toBool();
}

static str delegatePaintText(QModelIndex const& index, rcstr def) {
    str s = index.model()->data(index, QvDelegatePaintTextRole).toString();
    return s.isEmpty() ? def : s;
}

#define IF_DISABLE_DELEGATE if (disableDelegate(index))

//------------------------------------------------------------------------------

NullDelegate::NullDelegate(QObject* parent): super(parent) {
}

QWidget* NullDelegate::createEditor(QWidget*, const QStyleOptionViewItem&, const QModelIndex&) const {
    return NULL;
}

//------------------------------------------------------------------------------

ColorDelegate::ColorDelegate(QObject* parent): super(parent) {
}

QWidget* ColorDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const&, QModelIndex const& index) const {
    IF_DISABLE_DELEGATE return NULL;

    QComboBox *cb = new QComboBox(parent);

    str_lst colorNames = QColor::colorNames();
    foreach (rcstr name, colorNames) {
        QColor color(name);
        int cnt = cb->count();
        cb->insertItem(cnt, name);
        cb->setItemData(cnt, color, Qt::DecorationRole);
    }

    return cb;
}

void ColorDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const {
    QComboBox *cb = static_cast<QComboBox*>(editor);
    QColor color = index.model()->data(index, Qt::EditRole).value<QColor>();

    str_lst colorNames = QColor::colorNames();
    for_i (colorNames.size()) {
        if (QColor(colorNames[i]) == color) {
            cb->setCurrentIndex(i);
            return;
        }
    }
}

void ColorDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const {
    QComboBox *cb = static_cast<QComboBox*>(editor);
    model->setData(index, QColor(cb->currentText()));
}

void ColorDelegate::updateEditorGeometry(QWidget *editor, QStyleOptionViewItem const& option, QModelIndex const&) const {
    QRect rect = option.rect;
    rect.setWidth(editor->sizeHint().width());
    editor->setGeometry(rect);
}

void ColorDelegate::paint(QPainter* p, QStyleOptionViewItem const& option, QModelIndex const& index) const {
    QRect const& rect = option.rect;
    if (option.state & QStyle::State_Selected) p->fillRect(rect, option.palette.highlight());

    IF_DISABLE_DELEGATE return;

    QColor color = index.model()->data(index, Qt::EditRole).value<QColor>();
    p->setPen(color);
    p->drawText(rect,delegatePaintText(index,"◉"));
}

//------------------------------------------------------------------------------

RealDelegate::RealDelegate(QObject* parent): super(parent) {
}

QWidget* RealDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const&, QModelIndex const& index) const {
    IF_DISABLE_DELEGATE return NULL;

    QRealSpinBox *sb = new QRealSpinBox(parent);
    sb->setRange(0,1);
    sb->setDecimals(3);
    sb->setSingleStep(0.001);

    return sb;
}

void RealDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const {
    QRealSpinBox *sb = static_cast<QRealSpinBox*>(editor);
    qreal val = index.model()->data(index, Qt::EditRole).toReal();
    sb->setValue(val);
}

void RealDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const {
    QRealSpinBox *sb = static_cast<QRealSpinBox*>(editor);
    model->setData(index, sb->value());
}

//------------------------------------------------------------------------------

IntDelegate::IntDelegate(QObject* parent,int min_,int max_)
  : super(parent), min(min_), max(max_) {
}

QWidget* IntDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const&, QModelIndex const& index) const {
    IF_DISABLE_DELEGATE return NULL;

    QIntSpinBox *sb = new QIntSpinBox(parent);
    sb->setRange(min,max);

    return sb;
}

void IntDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const {
    QIntSpinBox *sb = static_cast<QIntSpinBox*>(editor);
    int val = index.model()->data(index, Qt::EditRole).toInt();
    sb->setValue(val);
}

void IntDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const {
    QIntSpinBox *sb = static_cast<QIntSpinBox*>(editor);
    model->setData(index, sb->value());
}

//------------------------------------------------------------------------------

  class Delegate: public QStyledItemDelegate {
    SUPER(Delegate,QStyledItemDelegate)
  public:
    Delegate(FileList&);

    QPoint iconPos(QStyleOptionViewItem const&) const;
    void paint(QPainter*, QStyleOptionViewItem const&, QModelIndex const&) const;

  private:
    FileList &fileList;
    QPixmap  icon;p
  };

  setMouseTracking(true);

//-----------------------------------------------------------------------------

FileList::Delegate::Delegate(FileList& fileList_): fileList(fileList_) {
  auto height = sizeHint(QStyleOptionViewItem(),QModelIndex()).height();
  icon = QPixmap(":/icon/rem").scaledToHeight(height,Qt::SmoothTransformation);
}

QPoint FileList::Delegate::iconPos(QStyleOptionViewItem const& option) const {
  WT(option.rect) WT(fileList.size())

  return QPoint(fileList.geometry().right() - icon.width(), option.rect.center().y() - icon.height()/2);
}

void FileList::Delegate::paint(QPainter* painter,
  QStyleOptionViewItem const& option, QModelIndex const& index) const
{
  super::paint(painter,option,index);
  if (option.state & QStyle::State_MouseOver)
    painter->drawPixmap(iconPos(option), icon);
}


//------------------------------------------------------------------------------
*/

// eof
