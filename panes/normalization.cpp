#include "normalization.h"
#include <QVBoxLayout>
#include <QRadioButton>

Normalization::Normalization(): super("Normalization") {
  auto sp = sizePolicy();
  sp.setVerticalStretch(0);
  setSizePolicy(sp);

  auto v = new VBox;
  setLayout(v);

  v->addWidget(new QRadioButton("None"));
  v->addWidget(new QRadioButton("To deltatime"));
  v->addWidget(new QRadioButton("To deltaMonitor"));
  v->addWidget(new QRadioButton("To background"));
  v->addStretch();
}

// eof
