#include "background.h"
#include "widgets/iconbutton.h"
#include "widgets/editcell.h"
#include <QHBoxLayout>
#include <QLabel>

Background::Background(): super("Background") {
  auto sp = sizePolicy();
  sp.setVerticalStretch(0);
  setSizePolicy(sp);

  auto h = new HBox;
  setLayout(h);

  h->addWidget(new IconButton(":/icon/background"));
  h->addWidget(new QLabel("Degree:"));
  h->addWidget(new EditCell());
  h->addWidget(new IconButton(":/icon/eye"));
}

// eof
