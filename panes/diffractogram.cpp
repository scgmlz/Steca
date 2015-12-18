#include "diffractogram.h"
#include <QVBoxLayout>
#include <QCheckBox>
#include <QPixmap>

Diffractogram::Diffractogram(): super("Diffractogram") {
  auto v = new VBox();
  setLayout(v);

  v->addWidget(new QCheckBox("From all images"));
}

// eof
