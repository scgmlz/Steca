//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/about.h
//! @brief     Defines class AboutBox
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef ABOUT_H
#define ABOUT_H

#include "gui/base/controls.h"
#include <QDialogButtonBox>

//! Modal dialog that informs about the software
class AboutBox : public QDialog {
public:
    AboutBox();
private:
    QVBoxLayout vb_;
    QHBoxLayout hb_;
    QLabel logo_;
    QLabel info_;
    QDialogButtonBox dbbox_ {QDialogButtonBox::Ok };
};

#endif // ABOUT_H
