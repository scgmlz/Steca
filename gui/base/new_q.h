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

QBoxLayout* HBoxLayout();
QBoxLayout* VBoxLayout();
QGridLayout* GridLayout();

QFile* OutputFile(
    const QString& name, QWidget* parent, const QString& path, bool check_overwrite=true);

QAction* Trigger(const QString& name, rcstr text, rcstr iconFile="");
QAction* Toggle(const QString& name, rcstr text, bool value, rcstr iconFile="");

QLabel* Label(const QString& name, rcstr text);
QLabel* Icon(const QString& name, rcstr fileName);

QLineEdit* LineDisplay(const QString& name, int ndigits, bool withDot);

QSpinBox* SpinBox(
    const QString& name, int ndigits, bool withDot, int min = INT_MIN, int max = INT_MAX);
QDoubleSpinBox* DoubleSpinBox(
    const QString& name, int ndigits, qreal min = LLONG_MIN, qreal max = LLONG_MAX);

QCheckBox* CheckBox(const QString& name, rcstr text);
QCheckBox* CheckBox(const QString& name, QAction*);

QToolButton* TextButton(const QString& name, QAction*);
QToolButton* IconButton(const QString& name, QAction*);

QRadioButton* RadioButton(const QString& name, rcstr text);

} // namespace newQ

#endif // NEW_Q_H
