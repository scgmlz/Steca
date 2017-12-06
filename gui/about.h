// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/about.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef ABOUT_H
#define ABOUT_H

#include "thehub.h"

#include <QDialog>

namespace gui {

class AboutBox : public QDialog {
    CLASS(AboutBox) SUPER(QDialog) public : AboutBox(QWidget*);

protected:
    void accept();

    QCheckBox *cbShowAtStartup_, *cbCheckUpdatesAtStartup_;
    QDoubleSpinBox *detDistance_, *detPixelSize_;
};
}
#endif // MAINWIN_H
