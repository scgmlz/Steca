#ifndef REFLECTIONS_H
#define REFLECTIONS_H

#include "pane.h"
#include <QListWidget>

class ReflectionList: public QListWidget {
  SUPER(ReflectionList,QListWidget)
public:
  ReflectionList();
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
