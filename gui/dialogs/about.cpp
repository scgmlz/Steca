//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/about.cpp
//! @brief     Implements class AboutBox
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/dialogs/about.h"
#include "gui/mainwin.h"
#include "manifest.h"
#include <QApplication>
#include <QDate>
#include <QLayout>
#include <QDialogButtonBox>

AboutBox::AboutBox()
    : QDialog(gGui, Qt::Dialog)
{
    QVBoxLayout vb_;
    QHBoxLayout hb_;
    QLabel logo_;
    QLabel info_;
    QDialogButtonBox dbbox_ {QDialogButtonBox::Ok };

    setWindowTitle(QString("About %1").arg(qApp->applicationName()));

    // vertical layout
    setLayout(&vb_);
    vb_.setSpacing(12);
    vb_.setSizeConstraint(QLayout::SetFixedSize);

    // logo and info
    vb_.addLayout(&hb_);
    hb_.setSpacing(12);
    logo_.setPixmap(QPixmap(":/icon/retroStier")
                    .scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    hb_.addWidget(&logo_);

#ifdef __x86_64__
    QString arch = "(64b)";
#else
    QString arch = "";
#endif
    info_.setText(QString(
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
    info_.setAlignment(Qt::AlignTop | Qt::AlignLeft);
    info_.setOpenExternalLinks(true);
#ifdef Q_OS_MAC
    // a smaller font (a hint found in Qt source code)
    info_.setFont(QToolTip::font());
#endif
    hb_.addWidget(&info_);

    vb_.addWidget(&dbbox_);

    connect(&dbbox_, &QDialogButtonBox::accepted, this, &QDialog::accept);
}
