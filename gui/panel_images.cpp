#include "panel_images.h"
//#include "widgets/iconbutton.h"
//#include "widgets/editcell.h"
//#include <QPainter>
//#include <QStylePainter>
//#include <QStyleOptionFrame>
//#include <QResizeEvent>

ImageWidget::ImageWidget() {
}

QSize ImageWidget::sizeHint() const {
  QSize s = size();
  lastHeight = s.height();
  s.setWidth(lastHeight);
  s.setHeight(super::sizeHint().height());
  return s;
}

void ImageWidget::resizeEvent(QResizeEvent* e) {
  super::resizeEvent(e);

  if (lastHeight!=height()) updateGeometry();
}

//------------------------------------------------------------------------------

PanelImages::PanelImages(MainWin& mainWin): super(mainWin,Qt::Horizontal) {
//  auto sp = sizePolicy();
//  sp.setHorizontalStretch(1);
//  sp.setVerticalStretch(1);
//  setSizePolicy(sp);

  auto v1 = vbox();
  box->addLayout(v1);

  v1->addWidget(w = new ImageWidget());

  auto hIcons = hbox();
  v1->addLayout(hIcons);

  hIcons->addWidget(iconButton(":/icon/updown"));
  hIcons->addWidget(iconButton(":/icon/leftright"));
  hIcons->addWidget(iconButton(":/icon/turnright"));
  hIcons->addWidget(iconButton(":/icon/turnleft"));
  hIcons->addStretch();

  auto v2 = vbox();
  box->addLayout(v2);

  v2->addWidget(label("Top:"));
  v2->addWidget(editCell());
  v2->addWidget(label("Bottom:"));
  v2->addWidget(editCell());
  v2->addWidget(label("Left:"));
  v2->addWidget(editCell());
  v2->addWidget(label("Right:"));
  v2->addWidget(editCell());

  auto v2h = hbox();
  v2->addLayout(v2h);
  v2h->addWidget(iconButton(":/icon/link"));
  v2h->addWidget(iconButton(":/icon/eye"));
  v2h->addStretch();

  v2->addStretch();
  box->addStretch();
}

// eof
