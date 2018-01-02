// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/popup/popup/about.h
//! @brief     Defines class AboutBox
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>

class QCheckBox;
class QDoubleSpinBox;

namespace gui {

class AboutBox : public QDialog {
public:
    AboutBox(QWidget*);

protected:
    void accept();

    QCheckBox *cbShowAtStartup_, *cbCheckUpdatesAtStartup_;
    QDoubleSpinBox *detDistance_, *detPixelSize_;
};

} // namespace gui

#endif // ABOUT_H
