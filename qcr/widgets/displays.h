//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/widgets/displays.h
//! @brief     Defines specializations of Qt display widgets (XIcon,...), and widgetUtils
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#ifndef DISPLAYS_H
#define DISPLAYS_H

#include <QLabel>
#include <QLineEdit>

namespace widgetUtils {
    void setWidth(QWidget* w, int ndigits, bool withDot);
}

//! QLabel displaying an icon, with no associated action.
class XIcon : public QLabel {
public:
    XIcon(const QString& fileName);
};

//! Read-only QLineEdit for number display.
class XLineDisplay : public QLineEdit {
public:
    XLineDisplay();
    XLineDisplay(int ndigits, bool withDot);
};

#endif // DISPLAYS_H
