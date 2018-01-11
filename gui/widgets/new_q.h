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
#include <QtGlobal> // to define Q_OS_WIN

class BoxWidget;


//! Contains OS specific constants that optimize the GUI appearance

namespace gui_cfg {

#ifdef Q_OS_WIN
uint const em4_2 = 8; // widget width for 4.2 numbers
uint const em4 = 5; // for 4 numbers
#else
uint const em4_2 = 6;
uint const em4 = 4;
#endif

} // namespace gui_cfg


//! Contains functions that return new Qt objects.

namespace newQ {

QAction* Trigger(rcstr text, rcstr iconFile="");
QAction* Toggle(rcstr text, bool value, rcstr iconFile="");

BoxWidget* Tab(QTabWidget* panel, rcstr title);

QBoxLayout* BoxLayout(Qt::Orientation);
QBoxLayout* HBoxLayout(); // horizontal box layout
QBoxLayout* VBoxLayout(); // vertical box layout

QGridLayout* GridLayout();

QLabel* Label(rcstr text);
QLabel* Icon(rcstr fileName);

QLineEdit* LineEdit(uint emWidth); // emWidth: measured in typographical (m)s
QLineEdit* LineDisplay(uint emWidth);

QSpinBox* SpinBox(uint ndigits, bool withDot, int min = INT_MIN, int max = INT_MAX);
QDoubleSpinBox* DoubleSpinBox(uint emWidth, qreal min = LLONG_MIN, qreal max = LLONG_MAX);

QCheckBox* CheckBox(rcstr text);
QCheckBox* CheckBox(QAction*);

QToolButton* TextButton(QAction*);
QToolButton* IconButton(QAction*);

QRadioButton* RadioButton(rcstr text);

} // namespace newQ

#endif // NEW_Q_H
