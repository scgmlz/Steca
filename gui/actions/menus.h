// ************************************************************************** //
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
// ************************************************************************** //

#ifndef MENUS_H
#define MENUS_H

#include <QMenuBar>

class Menus {
public:
    Menus() = delete;
    Menus(QMenuBar*);
    QMenu* dgram_;
    QMenu* image_;
    QMenu* output_;
private:
    QMenuBar* mbar_;
    QAction* separator() const;
    QMenu* actionsToMenu(const char* menuName, QList<QAction*> actions);
};

#endif // MENUS_H
