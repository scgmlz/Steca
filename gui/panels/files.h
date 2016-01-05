#ifndef FILES_H
#define FILES_H

#include "panel.h"
#include <QStyledItemDelegate>

namespace panel {

class FileList: public ListView {
  SUPER(FileList,ListView) Q_OBJECT
public:
  FileList(Session&);

protected:
  void selectionChanged(QItemSelection const&, QItemSelection const&);
signals:
  void selectedFile(str fileName); // fileName may be empty (no file)
public:
  void removeSelectedFile();

private:
  class Model: public QAbstractListModel {
    SUPER(Model,QAbstractListModel)
  public:
    Model(Session&);

    int rowCount(QModelIndex const&)      const;
    QVariant data(QModelIndex const&,int) const;

    Session &session;
  };

  class Delegate: public QStyledItemDelegate {
    SUPER(Delegate,QStyledItemDelegate)
  public:
    Delegate();

    void paint(QPainter*, QStyleOptionViewItem const&, QModelIndex const&) const;
  };

  Model    model;
  Delegate delegate;
};

class Files: public Panel {
  SUPER(Files,Panel)
public:
  Files(MainWin&);

private:
  FileList *fileList;
};

}

#endif
