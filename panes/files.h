#ifndef FILES_H
#define FILES_H

#include "pane.h"
#include <QTreeWidget>

class FileList: public QTreeWidget {
  SUPER(FileList,QTreeWidget)
public:
  FileList();

protected:
  void resizeEvent(QResizeEvent*);
};

class FileItem: public QTreeWidgetItem {
  SUPER(FileItem,QTreeWidgetItem)
public:
  FileItem(rcstr);
};

class Files: public Pane {
  SUPER(Files,Pane) Q_OBJECT
public:
  Files();

signals:

public slots:

private:
  FileList *l;
};

#endif
