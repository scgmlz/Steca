#include "files.h"
#include <QTreeWidget>
#include <QVBoxLayout>

Files::Files(): super("Files") {
  auto sp = sizePolicy();
  sp.setVerticalStretch(1);
  setSizePolicy(sp);

  auto v = new VBox();
  setLayout(v);

  v->addWidget(w = new QTreeWidget()); // TODO delegates
}

// eof
