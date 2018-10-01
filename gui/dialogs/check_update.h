//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/check_update.h
//! @brief     Defines class CheckUpdate
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef CHECK_UPDATE_H
#define CHECK_UPDATE_H

//! Queries whether there is a new version of this software, and shows message box.

class CheckUpdate {
public:
    CheckUpdate() = delete;
    CheckUpdate(class QWidget* parent);
};

#endif // CHECK_UPDATE_H
