// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/tab_mainframe.h
//! @brief     Defines class TabMainframe, and its dependences
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TAB_MAINFRAME_H
#define TAB_MAINFRAME_H

#include <QWidget>

//! Pure virtual base class for tabs in the Mainframe.

class TabMainframe : public QWidget {
public:
    virtual void render() = 0;
};

#endif // TAB_MAINFRAME_H
