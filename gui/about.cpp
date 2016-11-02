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
//! @authors   Rebecca Brydon, Jan Burle, Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "about.h"
#include "../manifest.h"
#include "config.h"
#include "gui_helpers.h"
#include "typ/typ_geometry.h"
#include <QApplication>
#include <QDialogButtonBox>
#include <QtMultimedia/QSound>

#ifdef Q_OS_MAC
#include <QToolTip>
#endif

namespace gui {
//------------------------------------------------------------------------------

AboutBox::AboutBox(QWidget *parent) : super(parent, Qt::Dialog) {
  Settings s(config_key::GROUP_CONFIG);

  int PAD = 12;

  setWindowTitle(str("About %1").arg(qApp->applicationName()));

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
          .arg(qApp->applicationName(), qApp->applicationVersion())
          .arg(QDate::currentDate().toString("yyyy"))
          .arg(STECA2_PAGES_URL));

  info->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  info->setOpenExternalLinks(true);
#ifdef Q_OS_MAC
  // a smaller font (a hint found in Qt source code)
  info->setFont(QToolTip::font());
#endif

  hb->addWidget(info);

  auto hline = []() {
    auto frame = new QFrame;
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);
    return frame;
  };

  // configuration

  vb->addWidget(hline());

  hb = hbox();
  vb->addLayout(hb);

  hb->addWidget(label("at startup:"));
  hb->addWidget((cbShowAtStartup_ = check("&show this dialog")));
  hb->addWidget((cbCheckUpdatesAtStartup_ = check("&check for update")));
  hb->addStretch();

  cbShowAtStartup_->setChecked(s.readBool(config_key::STARTUP_ABOUT, true));
  cbCheckUpdatesAtStartup_->setChecked(s.readBool(config_key::STARTUP_CHECK_UPDATE, true));

  vb->addWidget(hline());

  auto g = gridLayout();
  vb->addLayout(g);

  g->addWidget(label("default det. distance"), 0, 0);
  g->addWidget((detDistance_ = spinDoubleCell(6, typ::Geometry::MIN_DETECTOR_DISTANCE)), 0, 1);
  g->addWidget(label("default pixel size"),    1, 0);
  g->addWidget((detPixelSize_ = spinDoubleCell(6, typ::Geometry::MIN_DETECTOR_PIXEL_SIZE)), 1, 1);
  g->addColumnStretch();

  detPixelSize_->setDecimals(3);
  detDistance_->setValue(s.readReal(config_key::DET_DISTANCE, typ::Geometry::DEF_DETECTOR_DISTANCE));
  detPixelSize_->setValue(s.readReal(config_key::DET_PIX_SIZE, typ::Geometry::DEF_DETECTOR_PIXEL_SIZE));

  // buttons

  vb->addWidget(hline());

  auto bb = new QDialogButtonBox(QDialogButtonBox::Ok);
  vb->addWidget(bb);

  connect(bb, &QDialogButtonBox::accepted, this, &QDialog::accept);
}

void AboutBox::accept() {
  Settings s(config_key::GROUP_CONFIG);

  s.saveBool(config_key::STARTUP_ABOUT, cbShowAtStartup_->isChecked());
  s.saveBool(config_key::STARTUP_CHECK_UPDATE, cbCheckUpdatesAtStartup_->isChecked());

  s.saveReal(config_key::DET_DISTANCE, detDistance_->value());
  s.saveReal(config_key::DET_PIX_SIZE, detPixelSize_->value());

  super::accept();
}

void AboutBox::mouseDoubleClickEvent(QMouseEvent *) {
  QSound::play(":/HAL/good_evening");
}

//------------------------------------------------------------------------------
}
// eof
