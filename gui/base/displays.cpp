// ************************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/base/displays.cpp
//! @brief     Implements specializations of Qt display widgets (XIcon,...), and widgetUtils
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************************************

#include "gui/base/displays.h"
#include <QIcon>

void widgetUtils::setWidth(QWidget* w, int ndigits, bool withDot)
{
    int width = ndigits;
#ifdef Q_OS_WIN
    width += 1 + (withDot?1:0);
#endif
    w->setFixedWidth(width * w->fontMetrics().width('m'));
}

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
    widgetUtils::setWidth(this, ndigits, withDot);
}
