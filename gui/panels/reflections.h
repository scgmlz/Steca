#ifndef REFLECTIONS_H
#define REFLECTIONS_H

#include "panel.h"
#include <QListView>

namespace panel {

class ReflectionView: public ListView {
  SUPER(ReflectionView,ListView) Q_OBJECT
public:
  ReflectionView();
};

class Reflections: public BoxPanel {
  SUPER(Reflections,BoxPanel)
public:
  Reflections(MainWin&);

private:
  ReflectionView *reflectionView;
};

}

#endif
