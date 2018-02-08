// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/base/controls.cpp
//! @brief     Implements functions that return new Qt objects
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/base/controls.h"
#include "gui/base/displays.h"
#include "gui/console.h"
#include "gui/mainwin.h" // for _SLOT_
#include "core/def/numbers.h"
#include <QApplication> // for qApp for new Action
#include <QDebug>
#include <QtGlobal> // to define Q_OS_WIN

QAction* newT::Trigger(const QString& name, const QString& text, const QString& iconFile) {
    QAction* ret = new QAction(text, qApp);
    ret->setToolTip(text.toLower());
    if (iconFile!="")
        ret->setIcon(QIcon(iconFile));
    gConsole->learn(name, [ret](const QString& /*unused*/)->void { ret->trigger(); });
    QObject::connect(ret, &QAction::triggered, [name]()->void { gConsole->log(name+"!"); });
    return ret;
};

QAction* newT::Toggle(
    const QString& name, const QString& text, bool value, const QString& iconFile)
{
    QAction* ret = new QAction(text, qApp);
    ret->setToolTip(text.toLower());
    if (iconFile!="")
        ret->setIcon(QIcon(iconFile));
    ret->setCheckable(true);
    ret->setChecked(value);
    gConsole->learn(name, [ret](const QString& val)->void {
            if (val=="y")
                ret->setChecked(true);
            else if (val=="n")
                ret->setChecked(false);
            else
                qWarning() << "Invalid toggle setter argument '"+val+"'"; } );
    QObject::connect(ret, &QAction::toggled, [name](bool val)->void {
            gConsole->log(name+"="+(val ? "y" : "n")); });
    return ret;
};

XTextButton::XTextButton(QAction* action) {
    setDefaultAction(action);
    setToolButtonStyle(Qt::ToolButtonTextOnly);
}

XIconButton::XIconButton(QAction* action) {
    setDefaultAction(action);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
}

CSettable::CSettable(const QString& name, std::function<void(const QString&)> setter)
    : name_(name)
{
    gConsole->learn(name, setter);
}

CSettable::~CSettable() {
    gConsole->forget(name_);
}

// A QSpinBox controls an integer value. Therefore normally we need no extra width for a dot.
// However, sometimes we want to make a QSpinBox exactly as wide as a given QDoubleSpinBox,
// for nice vertical alignement. Then we use withDot=true.
CSpinBox::CSpinBox(const QString& _name, int ndigits, bool withDot, int min, int max)
    : CSettable(_name, [this](const QString& val)->void { setValue(val.toInt()); })
{
    widgetUtils::setWidth(this, ndigits, withDot);
    setMinimum(min);
    setMaximum(max > min ? max : min);
    connect(this, _SLOT_(QSpinBox, valueChanged, int), [this](int val)->void {
            gConsole->log2(hasFocus(), name()+"="+QString::number(val)); });
}

CDoubleSpinBox::CDoubleSpinBox(const QString& _name, int ndigits, qreal min, qreal max)
    : CSettable(_name, [this](const QString& val)->void { setValue(val.toDouble()); })
{
    widgetUtils::setWidth(this, ndigits, true);
    ASSERT(min<=max);
    setMinimum(min);
    setMaximum(max);
    connect(this, _SLOT_(QDoubleSpinBox, valueChanged, double), [this](double val)->void {
            gConsole->log2(hasFocus(), name()+"="+QString::number(val)); });
}

CCheckBox::CCheckBox(const QString& _name, QAction* action)
    : QCheckBox(action ? action->text().toLower() : "")
    , CSettable(_name, [this](const QString& val)->void { setChecked(val.toInt()); })
{
    if (!action)
        return;
    connect(this, &QCheckBox::toggled, [action](bool on) { action->setChecked(on); });
    connect(action, &QAction::toggled, [this](bool on) { setChecked(on); });
    setToolTip(action->toolTip());
    setChecked(action->isChecked());
    connect(this, _SLOT_(QCheckBox, stateChanged, int), [this](int val)->void {
            gConsole->log2(hasFocus(), name()+"="+QString::number(val)); });
}

CCheckBox::CCheckBox(const QString& name, const QString& text)
    : CCheckBox(name, {})
{
    setText(text);
}

CRadioButton::CRadioButton(const QString& _name, const QString& text)
    : QRadioButton(text)
    , CSettable(_name, [this](const QString& val)->void { setChecked(val.toInt()); })
{
    connect(this, _SLOT_(QRadioButton, toggled, bool), [this](bool val)->void {
            gConsole->log2(hasFocus(), name()+"="+(val?"y":"n")); });
}

CComboBox::CComboBox(const QString& _name, const QStringList& items)
    : CSettable(_name, [this](const QString& val)->void { setCurrentIndex(val.toInt()); })
{
    addItems(items);
    connect(this, _SLOT_(QComboBox, currentIndexChanged, int), [this](int val)->void {
            gConsole->log2(hasFocus(), name()+"="+QString::number(val)); });
}

CFileDialog::CFileDialog(QWidget *parent, const QString &caption,
                         const QString &directory, const QString &filter)
    : QFileDialog(parent, caption, directory, filter)
{
    gConsole->call("@push fdia");
    gConsole->learn("files", [this](const QString& val)->void {
            QStringList list = val.split(';');
            QString tmp = '"' + list.join("\" \"") + '"';
            selectFile(tmp);
        });
    gConsole->learn("close", [this](const QString& val)->void { accept(); });
}

CFileDialog::~CFileDialog() {
    gConsole->log("files="+selectedFiles().join(';'));
    gConsole->log("@close");
    gConsole->call("@pop");
}

int CFileDialog::exec() {
    int ret;
    if (gConsole->hasCommandsOnStack()) {
        qDebug() << "FileDialog OPEN";
        open();
        qDebug() << "FileDialog OPENED";
        gConsole->commandsFromStack();
        qDebug() << "FileDialog CLOSING";
        close();
        ret = QDialog::Accepted;
    } else {
        qDebug() << "FileDialog EXEC";
        ret = QFileDialog::exec();
        qDebug() << "FileDialog DONE";
    }
    return ret;
}
