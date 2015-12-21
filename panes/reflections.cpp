#include "reflections.h"
#include "widgets/iconbutton.h"
#include "widgets/metrics.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QStyledItemDelegate>
#include <QPainter>

ReflectionList::ReflectionList() {
  setMaximumWidth(textWidth("mmmmmmmmmmmmmm"));
}

// http://stackoverflow.com/questions/7175333/howto-create-delegate-for-qtreewidget#7178024
class IconDelegate: public QStyledItemDelegate {
  SUPER(IconDelegate,QStyledItemDelegate)
public:
  mutable QPixmap icon; mutable bool scaled = false; bool showAlways;

  IconDelegate(rcstr iconFile, bool showAlways_): super(), icon(iconFile), showAlways(showAlways_) {
  }

  QSize sizeHint(QStyleOptionViewItem const& option, QModelIndex const& index) const {
    QSize size = QStyledItemDelegate::sizeHint(option,index);

    if (!scaled) {
      icon = icon.scaledToHeight(size.height()-3,Qt::SmoothTransformation);
      scaled = true;
    }

    return size;
  }

  QPoint iconPos(QStyleOptionViewItem const& option) const {
    return QPoint(option.rect.center().x() - icon.width()/2,
                  option.rect.center().y() - icon.height()/2);
  }

  void paint(QPainter *painter, QStyleOptionViewItem const& option, QModelIndex const& index) const {
    QStyledItemDelegate::paint(painter, option, index);
    if(showAlways || option.state & QStyle::State_MouseOver)
      painter->drawPixmap(iconPos(option), icon);
  }
};

ReflectionItem::ReflectionItem(): super() {

}

Reflections::Reflections(): super("Reflections") {
  auto sp = sizePolicy();
  setSizePolicy(sp);

  auto v = new VBox;
  setLayout(v);

  auto h = new HBox;
  v->addLayout(h);

  h->addWidget(new QLabel("Output:"));
  h->addWidget(new IconButton(":/icon/peak"));
  h->addWidget(new IconButton(":/icon/reflect"));
  h->addWidget(new IconButton(":/icon/width"));
  h->addStretch();

  v->addWidget(l = new ReflectionList());

  l->setColumnCount(5);
//  l->header()->hide();
  l->setItemDelegateForColumn(0,new IconDelegate(":/icon/integral",true));
  l->setItemDelegateForColumn(2,new IconDelegate(":/icon/eye",true));
  l->setItemDelegateForColumn(3,new IconDelegate(":/icon/remove",false));

  l->addTopLevelItem(new ReflectionItem());
  l->addTopLevelItem(new ReflectionItem());
  l->addTopLevelItem(new ReflectionItem());
  l->addTopLevelItem(new ReflectionItem());

  l->resizeColumnToContents(0);
  l->resizeColumnToContents(1);
  l->resizeColumnToContents(2);
  l->resizeColumnToContents(3);
  l->resizeColumnToContents(4);
}

// eof
