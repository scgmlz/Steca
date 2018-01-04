// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/popup/about.cpp
//! @brief     Implements class AboutBox
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "popup/about.h"
#include "../manifest.h"
#include "cfg/gui_cfg.h"
#include "cfg/settings.h"
#include "data/geometry.h"
#include "widgets/widget_makers.h"
#include <QApplication>
#include <QDate>
#include <QDialogButtonBox>

#ifdef Q_OS_MAC
#endif

namespace gui {

AboutBox::AboutBox(QWidget* parent) : QDialog(parent, Qt::Dialog) {
    Settings s("config");

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

    auto logo = label("");
    logo->setPixmap(QPixmap(":/icon/retroStier")
                        .scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    hb->addWidget(logo);

#ifdef __x86_64__
    str arch = "(64b)";
#else
    str arch = "";
#endif

    auto info = label(str(
"<h4>%1 version %2 %3</h4>"
"<p>%4</p>"
"<p>Copyright: Forschungszentrum Jülich GmbH %5</p>"
"<p>In scientific publications, the use of Steca must be acknowledged"
" and documented by means of the following two citations:</p>"
"<p>C. Randau, U. Garbe, H.-G. Brokmeier,<br>"
"<i>StressTextureCalculator: a software tool to extract texture, strain"
" and microstructure information from area-detector measurements</i>,<br>"
"J. Appl. Cryst. 46 (2011).</p>"
"<p>R. E. Brydon, J. Burle, W. Gan, M. Hofmann, A. J. Soininen, J. Wuttke,<br>"
"<i>Stress and texture calculator Steca, version 2</i>,<br>"
"<a href='%6'>%6</a> (2018).</p>")
                      .arg(qApp->applicationName())
                      .arg(qApp->applicationVersion())
                      .arg(arch)
                      .arg(APPLICATION_CLAIM)
                      .arg(QDate::currentDate().toString("yyyy"))
                      .arg(STECA2_PAGES_URL)
        );

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

    cbShowAtStartup_->setChecked(s.readBool("startup about", true));
    cbCheckUpdatesAtStartup_->setChecked(s.readBool("startup check update", true));

    vb->addWidget(hline());

    auto g = gridLayout();
    vb->addLayout(g);

    // TODO put back
    //  g->addWidget(label("default det. distance"), 0, 0, Qt::AlignRight);
    g->addWidget(
        (detDistance_ = spinDoubleCell(gui_cfg::em4_2, Geometry::MIN_DETECTOR_DISTANCE)), 0,
        1);
    //  g->addWidget(label("default pixel size"), 1, 0, Qt::AlignRight);
    g->addWidget(
        (detPixelSize_ = spinDoubleCell(gui_cfg::em4_2, Geometry::MIN_DETECTOR_PIXEL_SIZE)), 1,
        1);
    g->setColumnStretch(g->columnCount(), 1);

    detPixelSize_->setDecimals(3);
    detDistance_->setValue(
        s.readReal("distance", Geometry::DEF_DETECTOR_DISTANCE));
    detPixelSize_->setValue(
        s.readReal("pixel size", Geometry::DEF_DETECTOR_PIXEL_SIZE));

    // TODO put back
    detDistance_->setVisible(false);
    detPixelSize_->setVisible(false);

    // buttons

    vb->addWidget(hline());

    auto bb = new QDialogButtonBox(QDialogButtonBox::Ok);
    vb->addWidget(bb);

    connect(bb, &QDialogButtonBox::accepted, this, &QDialog::accept);
}

void AboutBox::accept() {
    Settings s("config");

    s.saveBool("startup about", cbShowAtStartup_->isChecked());
    s.saveBool("startup check update", cbCheckUpdatesAtStartup_->isChecked());

    s.saveReal("distance", detDistance_->value());
    s.saveReal("pixel size", detPixelSize_->value());

    QDialog::accept();
}

} // namespace gui
