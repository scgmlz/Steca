#include "reflections.h"
#include "widgets/iconbutton.h"
#include "widgets/metrics.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

ReflectionList::ReflectionList() {
  setMaximumWidth(textWidth("mmmmmmmmmmmmmm"));
}

Reflections::Reflections(): super("Reflections") {
  auto sp = sizePolicy();
  setSizePolicy(sp);

  auto v = new VBox;
  setLayout(v);

  auto h = new HBox;
  v->addLayout(h);

  h->addWidget(new QLabel("Output:"));
  h->addWidget(new IconButton(":/icon/peak"));
  h->addWidget(new IconButton(":/icon/reflect"));
  h->addWidget(new IconButton(":/icon/width"));
  h->addStretch();

  v->addWidget(l = new ReflectionList());
}

// eof
