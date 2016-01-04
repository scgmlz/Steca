/*#include "images.h"
#include <QVBoxLayout>
#include <QPushButton>

ImageList::ImageList() {
//  setMaximumWidth(textWidth("mmmmmmmmmmmmmm"));
  addItems({
    "1      Ф: 32.4°",
    "2      Ф: 33.5°",
    "3      Ф: 34.0°",
    "4      Ф: 34.8°",
    "5      Ф: 35.1°",
  });
}

Images::Images(): super("Images") {
  auto sp = sizePolicy();
  sp.setHorizontalStretch(0);
  setSizePolicy(sp);

  auto v = new VBox;
  setLayout(v);

  v->addWidget(l = new ImageList());
  v->addWidget(new QPushButton("Combine..."));
}

// eof
*/
