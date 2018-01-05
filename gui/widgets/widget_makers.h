// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/widgets/widget_makers.h
//! @brief     Defines functions that return new widgets with Steca-standard settings
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
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
#include <QTabWidget>
#include <QToolButton>

class BoxWidget;

namespace wmaker {

BoxWidget* newTab(QTabWidget* panel, rcstr title);

QBoxLayout* newBoxLayout(Qt::Orientation);
QBoxLayout* newHBoxLayout(); // horizontal box layout
QBoxLayout* newVBoxLayout(); // vertical box layout

QGridLayout* newGridLayout();

QLabel* newLabel(rcstr);

QLineEdit* newLineEdit(uint emWidth); // emWidth: measured in typographical (m)s
QLineEdit* newLineDisplay(uint emWidth);

QSpinBox* newSpinBox(uint emWidth, int min = INT_MIN, int max = INT_MAX);
QDoubleSpinBox* newDoubleSpinBox(uint emWidth, qreal min = LLONG_MIN, qreal max = LLONG_MAX);

QCheckBox* newCheckBox(rcstr text);
QCheckBox* newCheckBox(QAction*);

QToolButton* newTextButton(QAction*);
QToolButton* newIconButton(QAction*);

QRadioButton* newRadioButton(rcstr text);

} // namespace wmaker

#endif // WIDGET_MAKERS_H
