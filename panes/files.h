#ifndef FILES_H
#define FILES_H

#include "pane.h"
#include <QListWidget>

class FileList: public QListWidget {
  SUPER(FileList,QListWidget)
public:
  FileList();
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
