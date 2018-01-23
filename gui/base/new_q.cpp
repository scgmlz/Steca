// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/base/new_q.cpp
//! @brief     Implements functions that return new Qt objects
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/base/new_q.h"
#include "gui/base/various_widgets.h"
#include "core/def/numbers.h"
#include <QApplication> // for qApp for new Action
#include <QtGlobal> // to define Q_OS_WIN

namespace {

static void setWidth(QWidget* w, int ndigits, bool withDot) {
    int width = ndigits;
#ifdef Q_OS_WIN
    width += 1 + (withDot?1:0);
#endif
    w->setMaximumWidth(width * w->fontMetrics().width('m'));
}

} // local methods

QAction* newQ::Trigger(rcstr text, rcstr iconFile) {
    QAction* ret = new QAction(text, qApp);
    ret->setToolTip(text.toLower());
    if (iconFile!="")
        ret->setIcon(QIcon(iconFile));
    return ret;
};

QAction* newQ::Toggle(rcstr text, bool value, rcstr iconFile) {
    QAction* ret = new QAction(text, qApp);
    ret->setToolTip(text.toLower());
    if (iconFile!="")
        ret->setIcon(QIcon(iconFile));
    ret->setCheckable(true);
    ret->setChecked(value);
    return ret;
};

BoxWidget* newQ::Tab(QTabWidget* panel, rcstr title) {
    auto ret = new BoxWidget(Qt::Vertical);
    panel->addTab(ret, title);
    return ret;
}

QBoxLayout* newQ::BoxLayout(Qt::Orientation orientation) {
    switch (orientation) {
    case Qt::Horizontal: return newQ::HBoxLayout();
    case Qt::Vertical: return newQ::VBoxLayout();
    }
    NEVER return nullptr;
}

QBoxLayout* newQ::HBoxLayout() {
    auto ret = new QHBoxLayout;
    ret->setSpacing(2);
    ret->setMargin(2);
    return ret;
}

QBoxLayout* newQ::VBoxLayout() {
    auto ret = new QVBoxLayout;
    ret->setSpacing(2);
    ret->setMargin(2);
    return ret;
}

QGridLayout* newQ::GridLayout() {
    auto ret = new QGridLayout;
    ret->setSpacing(2);
    ret->setMargin(2);
    return ret;
}

QLabel* newQ::Label(rcstr text) {
    return new QLabel(text);
}

QLabel* newQ::Icon(rcstr fileName) {
    auto ret = new QLabel;
    int h = ret->sizeHint().height();
    ret->setPixmap(QIcon(fileName).pixmap(QSize(h, h)));
    return ret;
}

QLineEdit* newQ::LineDisplay(int ndigits, bool withDot) {
    auto ret = new QLineEdit;
    setWidth(ret, ndigits, withDot);
    ret->setReadOnly(true);
    return ret;
}

QSpinBox* newQ::SpinBox(int ndigits, bool withDot, int min, int max) {
    auto ret = new QSpinBox;
    setWidth(ret, ndigits, withDot); // TODO: why would this ever have a dot
    ret->setMinimum(min);
    ret->setMaximum(max > min ? max : min);
    return ret;
}

QDoubleSpinBox* newQ::DoubleSpinBox(int ndigits, bool withDot, qreal min, qreal max) {
    auto ret = new QDoubleSpinBox;
    setWidth(ret, ndigits, withDot); // TODO: why would this ever have no dot
    ret->setMinimum(min);
    ret->setMaximum(max > min ? max : min);
    return ret;
}

QCheckBox* newQ::CheckBox(rcstr text) {
    return new QCheckBox(text);
}

QCheckBox* newQ::CheckBox(QAction* action) {
    if (!action)
        return new QCheckBox("");
    auto ret = new QCheckBox(action->text().toLower());
    QObject::connect(ret, &QCheckBox::toggled, [action](bool on) { action->setChecked(on); });
    QObject::connect(action, &QAction::toggled, [ret](bool on) { ret->setChecked(on); });
    ret->setToolTip(action->toolTip());
    ret->setChecked(action->isChecked());
    return ret;
}

QToolButton* newQ::TextButton(QAction* action) {
    auto ret = new QToolButton;
    ret->setDefaultAction(action);
    ret->setToolButtonStyle(Qt::ToolButtonTextOnly);
    return ret;
}

QToolButton* newQ::IconButton(QAction* action) {
    auto ret = new QToolButton;
    ret->setDefaultAction(action);
    ret->setToolButtonStyle(Qt::ToolButtonIconOnly);
    return ret;
}

QRadioButton* newQ::RadioButton(rcstr text) {
    return new QRadioButton(text);
}
