#ifndef FILES_H
#define FILES_H

#include "panel.h"
#include <QStyledItemDelegate>

namespace panel {

class FileList: public ListView {
  SUPER(FileList,ListView) Q_OBJECT
public:
  FileList(Session&); // TODO Session will be broadcast, eventually

protected:
  void selectionChanged(QItemSelection const&, QItemSelection const&);

public:
  void removeSelectedFile();

private:
  Session &session;

  class Delegate: public QStyledItemDelegate {
    SUPER(Delegate,QStyledItemDelegate)
  public:
    Delegate();

    void paint(QPainter*, QStyleOptionViewItem const&, QModelIndex const&) const;
  };

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
