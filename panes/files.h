#ifndef FILES_H
#define FILES_H

#include "pane.h"

class QTreeWidget;

class Files: public Pane {
  SUPER(Files,Pane) Q_OBJECT
public:
  Files();

signals:

public slots:

private:
  QTreeWidget *w;
};

#endif
