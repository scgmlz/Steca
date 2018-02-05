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
#include "gui/console.h"
#include "gui/mainwin.h"
#include "core/def/numbers.h"
#include <QApplication> // for qApp for new Action
#include <QDebug>
#include <QMessageBox>
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

QAction* newQ::Trigger(const QString& name, rcstr text, rcstr iconFile) {
    QAction* ret = new QAction(text, qApp);
    ret->setToolTip(text.toLower());
    if (iconFile!="")
        ret->setIcon(QIcon(iconFile));
    gConsole->registerAction(name, [ret]()->void { ret->trigger(); });
    return ret;
};

QAction* newQ::Toggle(const QString& name, rcstr text, bool value, rcstr iconFile) {
    QAction* ret = new QAction(text, qApp);
    ret->setToolTip(text.toLower());
    if (iconFile!="")
        ret->setIcon(QIcon(iconFile));
    ret->setCheckable(true);
    ret->setChecked(value);
    gConsole->registerAction(name, [ret]()->void { ret->toggle(); });
    return ret;
};

QFile* newQ::OutputFile(
    const QString& name, QWidget* parent, const QString& path, bool check_overwrite) {
    QFile* ret = new QFile(path);
    if (check_overwrite && ret->exists() &&
        QMessageBox::question(parent, "File exists", "Overwrite " + path + " ?") !=
        QMessageBox::Yes) {
        delete ret;
        return nullptr;
    }
    if (!ret->open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file for writing: " << path;
        return nullptr;
    }
    return ret;
}


CSettable::CSettable(const QString& name, std::function<void(const QString&)> setter)
    : name_(name) {
    gConsole->registerSetter(name, setter);
}

CSettable::~CSettable() {
    gConsole->deregisterSetter(name_);
}

CLineDisplay::CLineDisplay(const QString& name, int ndigits, bool withDot)
    : CSettable(name, [this](const QString& val)->void { setText(val); }) {
    setWidth(this, ndigits, withDot);
    setReadOnly(true);
}

// A QSpinBox controls an integer value. Therefore normally we need no extra width for a dot.
// However, sometimes we want to make a QSpinBox exactly as wide as a given QDoubleSpinBox,
// for nice vertical alignement. Then we use withDot=true.
CSpinBox::CSpinBox(const QString& name, int ndigits, bool withDot, int min, int max)
    : CSettable(name, [this](const QString& val)->void { setValue(val.toInt()); }) {
    setWidth(this, ndigits, withDot);
    setMinimum(min);
    setMaximum(max > min ? max : min);
}

CDoubleSpinBox::CDoubleSpinBox(const QString& name, int ndigits, qreal min, qreal max)
    : CSettable(name, [this](const QString& val)->void { setValue(val.toDouble()); }) {
    setWidth(this, ndigits, true);
    setMinimum(min);
    setMaximum(max > min ? max : min);
}

CCheckBox::CCheckBox(const QString& name, QAction* action)
    : QCheckBox(action ? action->text().toLower() : "")
    , CSettable(name, [this](const QString& val)->void { setChecked(val.toInt()); }) {
    if (!action)
        return;
    QObject::connect(this, &QCheckBox::toggled, [action](bool on) { action->setChecked(on); });
    QObject::connect(action, &QAction::toggled, [this](bool on) { setChecked(on); });
    setToolTip(action->toolTip());
    setChecked(action->isChecked());
}

CCheckBox::CCheckBox(const QString& name, rcstr text)
    : CCheckBox(name, {}) {
    setText(text);
}

CRadioButton::CRadioButton(const QString& name, rcstr text)
    : QRadioButton(text)
    , CSettable(name, [this](const QString& val)->void { setChecked(val.toInt()); }) {
}

CComboBox::CComboBox(const QString& name, const QStringList& items)
    : CSettable(name, [this](const QString& val)->void { setCurrentIndex(val.toInt()); }) {
    addItems(items);
}
