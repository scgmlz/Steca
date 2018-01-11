// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/popup/about.cpp
//! @brief     Implements class AboutBox
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/popup/about.h"
#include "../manifest.h"
#include "gui/cfg/settings.h"
#include "core/data/geometry.h"
#include "gui/widgets/new_q.h"
#include <QApplication>
#include <QDate>
#include <QDialogButtonBox>

#ifdef Q_OS_MAC
#endif


AboutBox::AboutBox(QWidget* parent) : QDialog(parent, Qt::Dialog) {
    Settings s("config");

    int PAD = 12;

    setWindowTitle(str("About %1").arg(qApp->applicationName()));

    // layout
    auto vb = newQ::VBoxLayout();
    setLayout(vb);

    vb->setSpacing(PAD);
    vb->setSizeConstraint(QLayout::SetFixedSize);

    // logo and info
    auto hb = newQ::HBoxLayout();
    vb->addLayout(hb);

    hb->setSpacing(PAD);

    auto logo = newQ::Label("");
    logo->setPixmap(QPixmap(":/icon/retroStier")
                        .scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    hb->addWidget(logo);

#ifdef __x86_64__
    str arch = "(64b)";
#else
    str arch = "";
#endif

    auto info = newQ::Label(str(
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

    auto bb = new QDialogButtonBox(QDialogButtonBox::Ok);
    vb->addWidget(bb);

    connect(bb, &QDialogButtonBox::accepted, this, &QDialog::accept);
}
