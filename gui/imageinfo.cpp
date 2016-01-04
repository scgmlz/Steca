/*#include "imageinfo.h"
#include "widgets/delegates.h"
#include <QHeaderView>
#include <QResizeEvent>
#include <QVBoxLayout>

ImageInfoList::ImageInfoList() {
//  setMaximumWidth(textWidth("mmmmmmmmmmmmmm"));
  setColumnCount(3);
  hideColumn(0);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  header()->hide();
  setItemDelegateForColumn(1,new CheckDelegate);
}

void ImageInfoList::resizeEvent(QResizeEvent* e) {
  int w = e->size().width();
  setColumnWidth(1,20);
  setColumnWidth(2,w-20);
}

ImageInfoItem::ImageInfoItem(rcstr s): super() {
  setText(2,s);
}

ImageInfo::ImageInfo(): super("ImageInfo") {
  auto sp = sizePolicy();
  sp.setVerticalStretch(1);
  setSizePolicy(sp);

  auto v = new VBox;
  setLayout(v);

  v->addWidget(l = new ImageInfoList());
  ImageInfoItem *fi;
  l->addTopLevelItem((fi=new ImageInfoItem("Oh-my-goodness: 13°")));
  l->addTopLevelItem(new ImageInfoItem("Panssarintorjunta: 0 mm"));
  l->addTopLevelItem(new ImageInfoItem("Ф: 34.8°"));
//  l->setCurrentItem(fi);
}
*/
// eof
