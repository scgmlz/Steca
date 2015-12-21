#include "image.h"
#include "widgets/iconbutton.h"
#include "widgets/editcell.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QStylePainter>
#include <QStyleOptionFrame>
#include <QResizeEvent>

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

Image::Image(): super(nullstr) {
  auto sp = sizePolicy();
  sp.setHorizontalStretch(1);
  sp.setVerticalStretch(1);
  setSizePolicy(sp);

  auto h = new HBox;
  setLayout(h);

  auto v1 = new VBox;
  h->addLayout(v1);

  v1->addWidget(w = new ImageWidget());

  auto hIcons = new HBox;
  v1->addLayout(hIcons);

  hIcons->addWidget(new IconButton(":/icon/updown"));
  hIcons->addWidget(new IconButton(":/icon/leftright"));
  hIcons->addWidget(new IconButton(":/icon/turnright"));
  hIcons->addWidget(new IconButton(":/icon/turnleft"));
  hIcons->addStretch();

  auto v2 = new VBox;
  h->addLayout(v2);

  v2->addWidget(new QLabel("Top:"));
  v2->addWidget(new EditCell());
  v2->addWidget(new QLabel("Bottom:"));
  v2->addWidget(new EditCell());
  v2->addWidget(new QLabel("Left:"));
  v2->addWidget(new EditCell());
  v2->addWidget(new QLabel("Right:"));
  v2->addWidget(new EditCell());

  auto v2h = new HBox;
  v2->addLayout(v2h);
  v2h->addWidget(new IconButton(":/icon/link"));
  v2h->addWidget(new IconButton(":/icon/eye"));
  v2h->addStretch();

  v2->addStretch();
  h->addStretch();
}

// eof
