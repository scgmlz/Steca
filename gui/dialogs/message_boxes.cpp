//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/message_boxes.cpp
//! @brief     Implements classes AboutBox, AddRangeBox
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/dialogs/message_boxes.h"
#include "gui/mainwin.h"
#include "manifest.h"
#include <QApplication>
#include <QDate>
#include <QLayout>
#include <QDialogButtonBox>

AboutBox::AboutBox()
    : QDialog(gGui, Qt::Dialog)
{
    auto* vb = new QVBoxLayout;
    auto* hb = new QHBoxLayout;
    auto* logo = new QLabel;
    auto* info = new QLabel;
    auto* dbbox = new QDialogButtonBox{QDialogButtonBox::Ok};

    setWindowTitle(QString("About %1").arg(qApp->applicationName()));

    // vertical layout
    setLayout(vb);
    vb->setSpacing(12);
    vb->setSizeConstraint(QLayout::SetFixedSize);

    // logo and info
    vb->addLayout(hb);
    hb->setSpacing(12);
    logo->setPixmap(QPixmap(":/icon/retroStier")
                    .scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    hb->addWidget(logo);

#ifdef __x86_64__
    QString arch = "(64b)";
#else
    QString arch = "";
#endif
    info->setText(QString(
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

    vb->addWidget(dbbox);

    connect(dbbox, &QDialogButtonBox::accepted, this, &QDialog::accept);
}


AddRangeBox::AddRangeBox(const QString& genre)
    : QDialog(gGui, Qt::Dialog)
{
    auto* vb = new QVBoxLayout;
    auto* hb = new QHBoxLayout;
    auto* info = new QLabel;
    auto* dbbox = new QDialogButtonBox{QDialogButtonBox::Ok};

    setWindowTitle("How to add a fit range");

    // vertical layout
    setLayout(vb);
    vb->setSpacing(12);
    vb->setSizeConstraint(QLayout::SetFixedSize);

    vb->addLayout(hb);
    hb->setSpacing(12);

    info->setText(QString(
"<p>To add a %1 fit range, mark it in the diffractogram:<br>"
"Move the cursor over the selected range while keeping the left mouse button pressed.</p>")
                      .arg(genre)
        );
    info->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    info->setOpenExternalLinks(true);
#ifdef Q_OS_MAC
    // a smaller font (a hint found in Qt source code)
    info->setFont(QToolTip::font());
#endif
    hb->addWidget(info);

    vb->addWidget(dbbox);

    connect(dbbox, &QDialogButtonBox::accepted, this, &QDialog::accept);
}
