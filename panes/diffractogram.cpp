#include "diffractogram.h"
#include <QVBoxLayout>
#include <QCheckBox>

Diffractogram::Diffractogram(): super("Diffractogram") {
  auto v = new VBox();
  setLayout(v);

  v->addWidget(new QCheckBox("From all images"));
}

// eof
