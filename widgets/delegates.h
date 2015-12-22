#ifndef DELEGATES_H
#define DELEGATES_H

#include "defs.h"
#include <QItemDelegate>
#include <QStyledItemDelegate>

// http://stackoverflow.com/questions/7175333/howto-create-delegate-for-qtreewidget#7178024
class IconDelegate: public QStyledItemDelegate {
  SUPER(IconDelegate,QStyledItemDelegate)
public:
  IconDelegate(rcstr iconFile, bool showAlways_);

  QSize sizeHint(QStyleOptionViewItem const&, QModelIndex const&) const;
  QPoint iconPos(QStyleOptionViewItem const& option) const;
  void paint(QPainter *painter, QStyleOptionViewItem const&, QModelIndex const&) const;

private:
  mutable QPixmap icon; mutable bool scaled = false; bool showAlways;
};

class CheckDelegate: public QItemDelegate {
    SUPER(CheckDelegate,QItemDelegate)
public:
    CheckDelegate(bool springy=false);

    QWidget *createEditor(QWidget*, QStyleOptionViewItem const&, QModelIndex const&) const;
    bool editorEvent(QEvent*, QAbstractItemModel*, QStyleOptionViewItem const&, QModelIndex const&);
    void paint(QPainter*, QStyleOptionViewItem const&, QModelIndex const&) const;

private:
    bool springy;	// mouse button up same as button down
};

class RadioDelegate: public QItemDelegate {
    SUPER(RadioDelegate,QItemDelegate)
public:
    RadioDelegate();

    QWidget *createEditor(QWidget*, QStyleOptionViewItem const&, QModelIndex const&) const;
    bool editorEvent(QEvent*, QAbstractItemModel*, QStyleOptionViewItem const&, QModelIndex const&);
    void paint(QPainter*, QStyleOptionViewItem const&, QModelIndex const&) const;
};

class ComboBoxDelegate: public QItemDelegate {
  SUPER(ComboBoxDelegate,QItemDelegate)
public:
  ComboBoxDelegate();

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  void setEditorData(QWidget *editor, const QModelIndex &index) const;
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
  std::vector<std::string> Items;

};

/*
//------------------------------------------------------------------------------

class NullDelegate: public QItemDelegate {
    SUPER(NullDelegate, QItemDelegate)
public:
    NullDelegate(QObject*);

    QWidget *createEditor(QWidget*, QStyleOptionViewItem const&, QModelIndex const&) const;
//	void setEditorData(QWidget*, QModelIndex const&) const;
//	void setModelData(QWidget*, QAbstractItemModel*, QModelIndex const&) const;
//	void updateEditorGeometry(QWidget*, QStyleOptionViewItem const&, QModelIndex const&) const;
};

//------------------------------------------------------------------------------

class ColorDelegate: public QItemDelegate {
    SUPER(ColorDelegate, QItemDelegate)
public:
    ColorDelegate(QObject*);

    QWidget *createEditor(QWidget*, QStyleOptionViewItem const&, QModelIndex const&) const;
    void setEditorData(QWidget*, QModelIndex const&) const;
    void setModelData(QWidget*, QAbstractItemModel*, QModelIndex const&) const;
    void updateEditorGeometry(QWidget*, QStyleOptionViewItem const&, QModelIndex const&) const;
    void paint(QPainter*, QStyleOptionViewItem const&, QModelIndex const&) const;
};

//------------------------------------------------------------------------------

class RealDelegate: public QItemDelegate {
    SUPER(RealDelegate, QItemDelegate)
public:
    RealDelegate(QObject*);

    QWidget *createEditor(QWidget*, QStyleOptionViewItem const&, QModelIndex const&) const;
    void setEditorData(QWidget*, QModelIndex const&) const;
    void setModelData(QWidget*, QAbstractItemModel*, QModelIndex const&) const;
};

//------------------------------------------------------------------------------

class IntDelegate: public QItemDelegate {
    SUPER(IntDelegate, QItemDelegate)
public:
    IntDelegate(QObject*,int min,int max);

    QWidget *createEditor(QWidget*, QStyleOptionViewItem const&, QModelIndex const&) const;
    void setEditorData(QWidget*, QModelIndex const&) const;
    void setModelData(QWidget*, QAbstractItemModel*, QModelIndex const&) const;

private:
    int min,max;
};

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
*/

#endif
