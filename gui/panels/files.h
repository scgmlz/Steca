#ifndef FILES_H
#define FILES_H

#include "panel.h"
#include <QStyledItemDelegate>

namespace panel {

class FileView: public ListView {
  SUPER(FileView,ListView) Q_OBJECT
public:
  FileView(Session&); // TODO Session will be broadcast, eventually

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

class Files: public BoxPanel {
  SUPER(Files,BoxPanel)
public:
  Files(MainWin&);

private:
  FileView *fileView;
};

}

#endif
