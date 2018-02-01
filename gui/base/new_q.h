// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/base/new_q.h
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
#include <QFile>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpinBox>
#include <QToolButton>

class BoxWidget;

//! Contains functions that return new Qt objects.

namespace newQ {

QFile* OutputFile(const QString& path);

QAction* Trigger(rcstr text, rcstr iconFile="");
QAction* Toggle(rcstr text, bool value, rcstr iconFile="");

QBoxLayout* HBoxLayout();
QBoxLayout* VBoxLayout();

QGridLayout* GridLayout();

QLabel* Label(rcstr text);
QLabel* Icon(rcstr fileName);

QLineEdit* LineDisplay(int ndigits, bool withDot);

QSpinBox* SpinBox(int ndigits, bool withDot, int min = INT_MIN, int max = INT_MAX);
QDoubleSpinBox* DoubleSpinBox(int ndigits, qreal min = LLONG_MIN, qreal max = LLONG_MAX);

QCheckBox* CheckBox(rcstr text);
QCheckBox* CheckBox(QAction*);

QToolButton* TextButton(QAction*);
QToolButton* IconButton(QAction*);

QRadioButton* RadioButton(rcstr text);

} // namespace newQ

#endif // NEW_Q_H
