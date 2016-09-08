// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      about.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "about.h"
#include "../manifest.h"
#include "gui_helpers.h"
#include <QApplication>
#include <QDialogButtonBox>
#include <QtMultimedia/QSound>
#ifdef Q_OS_MAC
#include <QToolTip>
#endif

namespace gui {
//------------------------------------------------------------------------------

str const GROUP_CONFIG("config");
str const KEY_STARTUP_ABOUT("startup about");
str const KEY_STARTUP_CHECK_UPDATE("startup check update");

AboutBox::AboutBox(QWidget *parent) : super(parent, Qt::Dialog) {
  Settings s(GROUP_CONFIG);

  int PAD = 12;

  setWindowTitle(str("About %1").arg(APPLICATION_NAME));

  // layout
  auto vb = vbox();
  setLayout(vb);

  vb->setSpacing(PAD);
  vb->setSizeConstraint(QLayout::SetFixedSize);

  // logo and info
  auto hb = hbox();
  vb->addLayout(hb);

  hb->setSpacing(PAD);

  auto logo = label(EMPTY_STR);
  logo->setPixmap(QPixmap(":/icon/retroStier")
                  .scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  hb->addWidget(logo);

  auto info = label(
      str("<h4>%1 ver. %2</h4>"
          "<p>StressTextureCalculator</p>"
          "<p>Copyright: Forschungszentrum Jülich GmbH %3</p>"
          "<p><a href='%4'>%4</a></p>")
          .arg(APPLICATION_NAME, APPLICATION_VERSION)
          .arg(QDate::currentDate().toString("yyyy"))
          .arg(STECA2_PAGES_URL));

  info->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  info->setOpenExternalLinks(true);
#ifdef Q_OS_MAC
  // a smaller font (a hint found in Qt source code)
  info->setFont(QToolTip::font());
#endif

  hb->addWidget(info);

  auto hline = new QFrame;
  hline->setFrameShape(QFrame::HLine);
  hline->setFrameShadow(QFrame::Sunken);
  vb->addWidget(hline);

  // configuration

  auto cb = vbox();
  vb->addLayout(cb);

  cb->addWidget((cbShowAtStartup_ = check("&Show this dialog at startup")));
  cb->addWidget((cbCheckUpdatesAtStartup_ = check("&Check for update at startup")));

  cbShowAtStartup_->setChecked(s.readBool(KEY_STARTUP_ABOUT, true));
  cbCheckUpdatesAtStartup_->setChecked(s.readBool(KEY_STARTUP_CHECK_UPDATE, true));

  // buttons

  hline = new QFrame;
  hline->setFrameShape(QFrame::HLine);
  hline->setFrameShadow(QFrame::Sunken);
  vb->addWidget(hline);

  auto bb = new QDialogButtonBox(QDialogButtonBox::Ok);
  vb->addWidget(bb);

  connect(bb, &QDialogButtonBox::accepted, this, &QDialog::accept);
}

void AboutBox::accept() {
  Settings s(GROUP_CONFIG);

  s.saveBool(KEY_STARTUP_ABOUT, cbShowAtStartup_->isChecked());
  s.saveBool(KEY_STARTUP_CHECK_UPDATE, cbCheckUpdatesAtStartup_->isChecked());

  super::accept();
}

void AboutBox::mouseDoubleClickEvent(QMouseEvent *) {
  QSound::play(":/HAL/good_evening");
}

//------------------------------------------------------------------------------
}
// eof
