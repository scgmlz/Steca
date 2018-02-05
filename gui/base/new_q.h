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

class XTextButton : public QToolButton {
public:
    XTextButton(QAction*);
};

class XIconButton : public QToolButton {
public:
    XIconButton(QAction*);
};

class XLineDisplay : public QLineEdit {
public:
    XLineDisplay(int ndigits, bool withDot);
};

//! Contains functions that return new Qt objects.

namespace newQ {

QBoxLayout* HBoxLayout();
QBoxLayout* VBoxLayout();
QGridLayout* GridLayout();

QLabel* Label(const QString& text);
QLabel* Icon(const QString& fileName);

QToolButton* TextButton(QAction* action);
QToolButton* IconButton(QAction* action);

QAction* Trigger(const QString& name, const QString& text, const QString& iconFile="");
QAction* Toggle(const QString& name, const QString& text, bool value, const QString& iconFile="");

QFile* OutputFile(
    const QString& name, QWidget* parent, const QString& path, bool check_overwrite=true);

} // namespace newQ

class CSettable {
public:
    CSettable(const QString& name, std::function<void(const QString&)> setter);
    ~CSettable();
    QString name() const { return name_; }
private:
    const QString name_;
};

class CSpinBox : public QSpinBox, private CSettable {
public:
    CSpinBox(const QString& name, int ndigits, bool withDot, int min = INT_MIN, int max = INT_MAX);
};

class CDoubleSpinBox : public QDoubleSpinBox, private CSettable {
public:
    CDoubleSpinBox(const QString& name, int ndigits, qreal min = LLONG_MIN, qreal max = LLONG_MAX);
};

class CCheckBox : public QCheckBox, private CSettable {
public:
    CCheckBox(const QString& name, QAction*);
    CCheckBox(const QString& name, const QString& text);
};

class CRadioButton : public QRadioButton, private CSettable {
public:
    CRadioButton(const QString& name, const QString& text);
};

class CComboBox : public QComboBox, private CSettable {
public:
    CComboBox(const QString& name, const QStringList& items = {});
};

#endif // NEW_Q_H
