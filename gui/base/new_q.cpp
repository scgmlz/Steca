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
#include "gui/thehub.h" // for _SLOT_
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

QToolButton* newQ::TextButton(QAction* action) { return new XTextButton(action); }
QToolButton* newQ::IconButton(QAction* action) { return new XIconButton(action); }

QLabel* newQ::Label(rcstr text) {
    return new QLabel(text);
}

QLabel* newQ::Icon(rcstr fileName) {
    auto ret = new QLabel;
    int h = ret->sizeHint().height();
    ret->setPixmap(QIcon(fileName).pixmap(QSize(h, h)));
    return ret;
}

QAction* newQ::Trigger(const QString& name, rcstr text, rcstr iconFile) {
    QAction* ret = new QAction(text, qApp);
    ret->setToolTip(text.toLower());
    if (iconFile!="")
        ret->setIcon(QIcon(iconFile));
    gConsole->registerAction(name, [ret]()->void { ret->trigger(); });
    QObject::connect(ret, &QAction::triggered, [name]()->void { gConsole->log(name+"!"); });
    return ret;
};

QAction* newQ::Toggle(const QString& name, rcstr text, bool value, rcstr iconFile) {
    QAction* ret = new QAction(text, qApp);
    ret->setToolTip(text.toLower());
    if (iconFile!="")
        ret->setIcon(QIcon(iconFile));
    ret->setCheckable(true);
    ret->setChecked(value);
    gConsole->registerSetter(name, [ret](const QString& val)->void {
            if (val=="y")
                ret->setChecked(true);
            else if (val=="n")
                ret->setChecked(false);
            else
                qWarning() << "Invalid toggle setter argument '"+val+"'"; } );
    QObject::connect(ret, &QAction::triggered, [ret,name]()->void {
            gConsole->log(name+"="+(ret->isChecked() ? "y" : "n")); });
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

XTextButton::XTextButton(QAction* action) {
    setDefaultAction(action);
    setToolButtonStyle(Qt::ToolButtonTextOnly);
}

XIconButton::XIconButton(QAction* action) {
    setDefaultAction(action);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
}

XLineDisplay::XLineDisplay(int ndigits, bool withDot) {
    setWidth(this, ndigits, withDot);
    setReadOnly(true);
}


CSettable::CSettable(const QString& name, std::function<void(const QString&)> setter)
    : name_(name) {
    gConsole->registerSetter(name, setter);
}

CSettable::~CSettable() {
    gConsole->deregisterSetter(name_);
}

// A QSpinBox controls an integer value. Therefore normally we need no extra width for a dot.
// However, sometimes we want to make a QSpinBox exactly as wide as a given QDoubleSpinBox,
// for nice vertical alignement. Then we use withDot=true.
CSpinBox::CSpinBox(const QString& _name, int ndigits, bool withDot, int min, int max)
    : CSettable(_name, [this](const QString& val)->void { setValue(val.toInt()); }) {
    setWidth(this, ndigits, withDot);
    setMinimum(min);
    setMaximum(max > min ? max : min);
    connect(this, _SLOT_(QSpinBox, valueChanged, int), [this](int val)->void {
            gConsole->log(name()+"="+QString::number(val)); });
}

CDoubleSpinBox::CDoubleSpinBox(const QString& _name, int ndigits, qreal min, qreal max)
    : CSettable(_name, [this](const QString& val)->void { setValue(val.toDouble()); }) {
    setWidth(this, ndigits, true);
    setMinimum(min);
    setMaximum(max > min ? max : min);
    connect(this, _SLOT_(QDoubleSpinBox, valueChanged, double), [this](double val)->void {
            gConsole->log(name()+"="+QString::number(val)); });
}

CCheckBox::CCheckBox(const QString& _name, QAction* action)
    : QCheckBox(action ? action->text().toLower() : "")
    , CSettable(_name, [this](const QString& val)->void { setChecked(val.toInt()); }) {
    if (!action)
        return;
    connect(this, &QCheckBox::toggled, [action](bool on) { action->setChecked(on); });
    connect(action, &QAction::toggled, [this](bool on) { setChecked(on); });
    setToolTip(action->toolTip());
    setChecked(action->isChecked());
    connect(this, _SLOT_(QCheckBox, stateChanged, int), [this](int val)->void {
            gConsole->log(name()+"="+QString::number(val)); });
}

CCheckBox::CCheckBox(const QString& name, rcstr text)
    : CCheckBox(name, {}) {
    setText(text);
}

CRadioButton::CRadioButton(const QString& _name, rcstr text)
    : QRadioButton(text)
    , CSettable(_name, [this](const QString& val)->void { setChecked(val.toInt()); }) {
    connect(this, _SLOT_(QRadioButton, toggled, bool), [this](bool val)->void {
            gConsole->log(name()+"="+(val?"y":"n")); });
}

CComboBox::CComboBox(const QString& _name, const QStringList& items)
    : CSettable(_name, [this](const QString& val)->void { setCurrentIndex(val.toInt()); }) {
    addItems(items);
    connect(this, _SLOT_(QComboBox, currentIndexChanged, int), [this](int val)->void {
            gConsole->log(name()+"="+QString::number(val)); });
}
