#include "files.h"
#include "widgets/metrics.h"
#include <QVBoxLayout>

FileList::FileList() {
  setMaximumWidth(textWidth("mmmmmmmmmmmmmm"));
  addItems({"m0001","m0002"});
}

Files::Files(): super("Files") {
  auto sp = sizePolicy();
  sp.setVerticalStretch(1);
  setSizePolicy(sp);

  auto v = new VBox;
  setLayout(v);

  v->addWidget(l = new FileList());
}

// eof
