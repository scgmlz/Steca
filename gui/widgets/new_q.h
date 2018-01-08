// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/widgets/new_q.h
//! @brief     Defines functions that return new Qt objects
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef NEW_Q_H
#define NEW_Q_H

#include "core/typ/str.h"
#include <QAction>
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

//! Contains functions that return new Qt objects with Steca-standard settings
namespace newQ {

QAction* Trigger(rcstr text, rcstr iconFile="");
QAction* Toggle(rcstr text, bool value, rcstr iconFile="");

BoxWidget* Tab(QTabWidget* panel, rcstr title);

QBoxLayout* BoxLayout(Qt::Orientation);
QBoxLayout* HBoxLayout(); // horizontal box layout
QBoxLayout* VBoxLayout(); // vertical box layout

QGridLayout* GridLayout();

QLabel* Label(rcstr);

QLineEdit* LineEdit(uint emWidth); // emWidth: measured in typographical (m)s
QLineEdit* LineDisplay(uint emWidth);

QSpinBox* SpinBox(uint emWidth, int min = INT_MIN, int max = INT_MAX);
QDoubleSpinBox* DoubleSpinBox(uint emWidth, qreal min = LLONG_MIN, qreal max = LLONG_MAX);

QCheckBox* CheckBox(rcstr text);
QCheckBox* CheckBox(QAction*);

QToolButton* TextButton(QAction*);
QToolButton* IconButton(QAction*);

QRadioButton* RadioButton(rcstr text);

} // namespace newQ

#endif // NEW_Q_H
