#include "panel.h"
#include "layout_helpers.h"

#include <QLayout>
#include <QGroupBox>

Panel::Panel() {
  setLayout((v = vbox()));
  // white space around panels provided elsewhere
  v->setContentsMargins(0,0,0,0);
}

void Panel::groupBox(rcstr title) {
  v->addWidget(new QGroupBox(title));
}

// eof
