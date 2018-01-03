// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/widgets/widget_makers.h
//! @brief     Defines functions that return new widgets with Steca-standard settings
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef WIDGET_MAKERS_H
#define WIDGET_MAKERS_H

#include "typ/str.h"
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpinBox>
#include <QToolButton>

QBoxLayout* boxLayout(Qt::Orientation);
QBoxLayout* hbox(); // horizontal box layout
QBoxLayout* vbox(); // vertical box layout

QGridLayout* gridLayout();

QLabel* label(rcstr);

QLineEdit* editCell(uint emWidth); // emWidth: measured in typographical (m)s
QLineEdit* readCell(uint emWidth);

QSpinBox* spinCell(uint emWidth, int min = INT_MIN, int max = INT_MAX);
QDoubleSpinBox* spinDoubleCell(uint emWidth, qreal min = LLONG_MIN, qreal max = LLONG_MAX);

QCheckBox* check(rcstr text);
QCheckBox* check(QAction*);

QToolButton* textButton(QAction*);
QToolButton* iconButton(QAction*);

QRadioButton* radioButton(rcstr text);

#endif // WIDGET_MAKERS_H
