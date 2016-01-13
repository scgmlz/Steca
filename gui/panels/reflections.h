#ifndef REFLECTIONS_H
#define REFLECTIONS_H

#include "panel.h"
#include <QListView>

namespace panel {

class ReflectionList: public ListView {
  SUPER(ReflectionList,ListView) Q_OBJECT
public:
  ReflectionList();
};

class Reflections: public BoxPanel {
  SUPER(Reflections,BoxPanel)
public:
  Reflections(MainWin&);

private:
  ReflectionList *reflectionList;
};

}

#endif
