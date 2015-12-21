#ifndef REFLECTIONS_H
#define REFLECTIONS_H

#include "pane.h"
#include <QTreeWidget>

class ReflectionList: public QTreeWidget {
  SUPER(ReflectionList,QTreeWidget)
public:
  ReflectionList();
};

class ReflectionItem: public QTreeWidgetItem {
  SUPER(ReflectionItem,QTreeWidgetItem)
public:
  ReflectionItem();
};

class Reflections: public Pane {
  SUPER(Reflections,Pane) Q_OBJECT
public:
  Reflections();

signals:

public slots:

private:
  ReflectionList *l;
};

#endif
