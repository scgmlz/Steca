#include "files.h"
#include <QTreeWidget>
#include <QVBoxLayout>

Files::Files(): super("Files") {
  auto v = new VBox();
  setLayout(v);

  v->addWidget(w = new QTreeWidget()); // TODO delegates
}

// eof
