//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/actions/menus.h
//! @brief     Defines class Menus
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef MENUS_H
#define MENUS_H

#include "qcr/widgets/views.h"

//! Submenus and menu entries of the MainWin menu bar.

class Menus {
public:
    Menus() = delete;
    Menus(QMenuBar*);
    QcrMenu* dgram_;
    QcrMenu* image_;
    QcrMenu* export_;
private:
    QMenuBar* mbar_;
    QAction* separator() const;
    QcrMenu* actionsToMenu(const char* menuName, QList<QAction*> actions);
};

#endif // MENUS_H
