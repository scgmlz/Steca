//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/widgets/displays.cpp
//! @brief     Implements specializations of Qt display widgets (XIcon,...), and widgetUtils
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#include "displays.h"
#include "qcr/base/string_ops.h"
#include <QIcon>

XIcon::XIcon(const QString& fileName)
{
    int h = sizeHint().height();
    setPixmap(QIcon(fileName).pixmap(QSize(h, h)));
}

XLineDisplay::XLineDisplay()
{
    setReadOnly(true);
}

XLineDisplay::XLineDisplay(int ndigits, bool withDot)
    : XLineDisplay()
{
    strOp::setWidth(this, ndigits, withDot);
}
