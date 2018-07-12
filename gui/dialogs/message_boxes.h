//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/message_boxes.h
//! @brief     Defines classes AboutBox, AddRangeBox
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>

//! Modal dialog that informs about the software
class AboutBox : public QDialog {
public:
    AboutBox();
};

//! Modal dialog that informs how to add a fit range
class AddRangeBox : public QDialog {
public:
    AddRangeBox() = delete;
    AddRangeBox(const QString&);
};

#endif // ABOUT_H
