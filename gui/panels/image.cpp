#include "image.h"
#include "mainwin.h"

namespace panel {

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

Image::Image(MainWin& mainWin): super(mainWin,"",Qt::Horizontal) {

  auto v1 = vbox();
  box->addLayout(v1);

  v1->addWidget(w = new ImageWidget());

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

  v2->addWidget(iconButton(mainWin.actImagesLink));
  v2->addWidget(iconButton(mainWin.actImagesEye));

  v2->addStretch();

  v2->addWidget(iconButton(mainWin.actImagesUpDown));
  v2->addWidget(iconButton(mainWin.actImagesLeftRight));
  v2->addWidget(iconButton(mainWin.actImagesTurnRight));
  v2->addWidget(iconButton(mainWin.actImagesTurnLeft));

  box->addStretch();
}

}

// eof
