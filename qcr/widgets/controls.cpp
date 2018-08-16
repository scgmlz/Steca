//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/widgets/controls.cpp
//! @brief     Implements enhanced control widgets like QcrAction, QcrSpinBox, and many others
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#include "qcr/widgets/controls.h"
#include "qcr/base/debug.h"
#include "qcr/base/qcrexception.h"
#include "qcr/base/string_ops.h"
#include "qcr/engine/console.h"
#include <QApplication> // for qApp for new Action
#include <iostream> // debug

#define _SLOT_(Class, method, argType) static_cast<void (Class::*)(argType)>(&Class::method)

//  Action classes

//  ***********************************************************************************************
//! @class QcrAction

QcrAction::QcrAction(const QString& text)
    : QAction(text, qApp)
    , tooltip_(text)
{}

bool QcrAction::hasFocus()
{
    for (const QWidget* w: associatedWidgets())
        if (w->hasFocus())
            return true;
    return false;
}

//  ***********************************************************************************************
//! @class QcrTrigger

QcrTrigger::QcrTrigger(const QString& rawname, const QString& text, const QString& iconFile)
    : QcrAction {text}
    , QcrSettable {*this, rawname}
{
    //QAction::setObjectName(name());
    if (iconFile!="")
        setIcon(QIcon(iconFile));
    connect(this, &QAction::triggered, [this]()->void {
            gConsole->log(name()); });
    connect(this, &QAction::changed, [this]()->void {
            QString txt = tooltip_;
            if (!isEnabled())
                txt += "\nThis trigger is currently inoperative.";
            setToolTip(txt); });
};

QcrTrigger::QcrTrigger(
    const QString& name, const QString& text, const QString& iconFile, const QKeySequence& shortcut)
    : QcrTrigger {name, text, iconFile}
{
    setShortcut(shortcut);
}

void QcrTrigger::executeConsoleCommand(const QString& arg)
{
    if (arg!="")
        throw QcrException("Found unexpected argument to trigger command");
    trigger();
}

//  ***********************************************************************************************
//! trigger button classes

QcrTextTriggerButton::QcrTextTriggerButton(QcrTrigger* action)
    : QcrMixin(*this, action->name()+"Btn")
{
    setDefaultAction(action);
    setToolButtonStyle(Qt::ToolButtonTextOnly);
    setRemake([=](){action->remake();});
}

QcrIconTriggerButton::QcrIconTriggerButton(QcrTrigger* action)
    : QcrMixin(*this, action->name()+"Btn")
{
    setDefaultAction(action);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setRemake([=](){action->remake();});
}

//  ***********************************************************************************************
//! @class QcrToggle

QcrToggle::QcrToggle(const QString& rawname, const QString& text, bool on,
                     const QString& iconFile, const QKeySequence& shortcut)
    : QcrAction {text}
    , QcrControl<bool> {*this, rawname, on}
{
    initToggle(iconFile, shortcut);
}

QcrToggle::QcrToggle(const QString& rawname, QcrCell<bool>* cell, const QString& text,
                     const QString& iconFile, const QKeySequence& shortcut)
    : QcrAction {text}
    , QcrControl<bool> {*this, rawname, cell}

{
    initToggle(iconFile, shortcut);
}

void QcrToggle::initToggle(const QString& iconFile, const QKeySequence& shortcut)
{
    setShortcut(shortcut);
    setCheckable(true);
    //QAction::setObjectName(QcrSettable::name());
    if (iconFile!="")
        setIcon(QIcon(iconFile));
    initControl();
    connect(this, &QAction::toggled, this, [this](bool val){
            //qDebug()<<"TOGGLE "<<name()<<"toggled";
            onChangedValue(hasFocus()&&!softwareCalling_, val);});
    connect(this, &QAction::changed, [this]()->void {
            // Called upon any property change.
            // Also called when this is toggled (https://bugreports.qt.io/browse/QTBUG-68213)
            //qDebug()<<"TOGGLE "<<name()<<"changed";
            QString txt = tooltip_;
            if (!isEnabled())
                txt += "\nThis toggle is currently inoperative.";
            else if (isChecked())
                txt += "\nThis toggle is currently checked. Click to uncheck.";
            else
                txt += "\nThis toggle is currently unchecked. Click to check.";
            setToolTip(txt); });
};

//  ***********************************************************************************************
//! toggle button classes

QcrTextToggleButton::QcrTextToggleButton(QcrToggle* action)
    : QcrMixin(*this, action->name()+"Btn")
{
    setDefaultAction(action);
    setToolButtonStyle(Qt::ToolButtonTextOnly);
    setRemake([=](){action->remake();});
}

QcrIconToggleButton::QcrIconToggleButton(QcrToggle* action)
    : QcrMixin(*this, action->name()+"Btn")
{
    setDefaultAction(action);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setRemake([=](){action->remake();});
}

//  Control widget classes with console connection

//  ***********************************************************************************************
//! @class QcrSpinBox
//!
//! A QSpinBox controls an integer value. Therefore normally we need no extra width for a dot.
//! However, sometimes we want to make a QSpinBox exactly as wide as a given QDoubleSpinBox,
//! for nice vertical alignement. Then we use withDot=true.
//!
//! The signal QSpinBox::valueChanged cannot be used to trigger lengthy computations
//! because it will cause duplicate incrementation. A workaround is described at
//! https://forum.qt.io/topic/89011. Here, we explicitly deal with editingFinished and
//! mouse release events.

QcrSpinBox::QcrSpinBox(const QString& _name, QcrCell<int>* cell, int ndigits,
                       bool withDot, int min, int max, const QString& tooltip)
    : QcrControl<int> {*this, _name, cell}
{
    initSpinBox(ndigits, withDot, min, max, tooltip);
}

void QcrSpinBox::initSpinBox(int ndigits, bool withDot, int min, int max, const QString& tooltip)
{
    strOp::setWidth(this, 2+ndigits, withDot);
    ASSERT(min<=max);
    setMinimum(min);
    setMaximum(max);
    if (tooltip!="")
        setToolTip(tooltip);
    initControl();
    connect(this, &QSpinBox::editingFinished, this, &QcrSpinBox::reportChange);
    connect(this, _SLOT_(QSpinBox,valueChanged,int), [this](int val)->void {
            if(!hasFocus())
                onChangedValue(hasFocus()&&!softwareCalling_, val); });
}

void QcrSpinBox::mouseReleaseEvent(QMouseEvent* event)
{
    QSpinBox::mouseReleaseEvent(event);
    reportChange();
}

void QcrSpinBox::reportChange()
{
    int val = value();
    if (val == reportedValue_)
        return;
    onChangedValue(hasFocus()&&!softwareCalling_, val);
}

void QcrSpinBox::executeConsoleCommand(const QString& arg)
{
    int val = strOp::to_i(arg);
    programaticallySetValue(val);
}

//  ***********************************************************************************************
//! @class QcrDoubleSpinBox

QcrDoubleSpinBox::QcrDoubleSpinBox(
    const QString& _name, QcrCell<double>* cell, int nDigits, int nDecimals,
    double min, double max, const QString& tooltip)
    : QcrControl<double> {*this, _name, cell}
{
    initDoubleSpinBox(nDigits, nDecimals, min, max, tooltip);
}

void QcrDoubleSpinBox::initDoubleSpinBox(
    int nDigits, int nDecimals, double min, double max, const QString& tooltip)
{
    strOp::setWidth(this, nDigits, true);
    setDecimals(nDecimals);
    ASSERT(min<=max);
    setMinimum(min);
    setMaximum(max);
    if (tooltip!="")
        setToolTip(tooltip);
    initControl();
    connect(this, &QDoubleSpinBox::editingFinished, this, &QcrDoubleSpinBox::reportChange);
    connect(this, _SLOT_(QDoubleSpinBox,valueChanged,double), [this](double val)->void {
            if(!hasFocus())
                onChangedValue(hasFocus()&&!softwareCalling_, val); });
}

void QcrDoubleSpinBox::mouseReleaseEvent(QMouseEvent* event)
{
    QDoubleSpinBox::mouseReleaseEvent(event);
    reportChange();
}

void QcrDoubleSpinBox::reportChange()
{
    double val = value();
    if (val == reportedValue_)
        return;
    onChangedValue(hasFocus()&&!softwareCalling_, val);
}

void QcrDoubleSpinBox::executeConsoleCommand(const QString& arg)
{
    double val = strOp::to_d(arg);
    programaticallySetValue(val);
}

//  ***********************************************************************************************
//! @class QcrCheckBox

QcrCheckBox::QcrCheckBox(const QString& _name, const QString& text, QcrCell<bool>* cell)
    : QCheckBox {text}
    , QcrControl<bool> {*this, _name, cell}
{
    initControl();
    connect(this, _SLOT_(QCheckBox,stateChanged,int), [this](int val)->void {
            onChangedValue(hasFocus()&&!softwareCalling_, (bool)val); });
}

//  ***********************************************************************************************
//! @class QcrRadioButton

QcrRadioButton::QcrRadioButton(const QString& _name, const QString& text, bool val)
    : QRadioButton {text}
    , QcrControl<bool> {*this, _name, val}
{
    initControl();
    setAutoExclusive(false); // TODO provide int-valued Qcr wrapper for exclusive radio buttons
    connect(this, _SLOT_(QRadioButton,toggled,bool), [this,_name](bool val)->void {
            onChangedValue(hasFocus()&&!softwareCalling_, val); });
}

QcrRadioButton::QcrRadioButton(const QString& _name, const QString& text, QcrCell<bool>* cell)
    : QRadioButton {text}
    , QcrControl<bool> {*this, _name, cell}
{
    initControl();
    setAutoExclusive(false);
    connect(this, _SLOT_(QRadioButton,toggled,bool), [this](bool val)->void {
            onChangedValue(hasFocus()&&!softwareCalling_, val); });
}

//  ***********************************************************************************************
//! @class QcrLineEdit

QcrLineEdit::QcrLineEdit(const QString& _name, const QString& val)
    : QcrControl<QString> {*this, _name, val}
{
    initControl();
    // For unknown reason, hasFocus() is not always false when setText is called programmatically;
    // therefore we must use another criterion to distinuish user actions from other calls.
    // The following works, but has the drawback that a user action is logged not only as such,
    // but also in a second line as if there were an indirect call.
    connect(this, _SLOT_(QLineEdit,textEdited,const QString&),
            [this](const QString& val)->void {
                onChangedValue(hasFocus()&&!softwareCalling_, val); });
    connect(this, _SLOT_(QLineEdit,textChanged,const QString&),
            [this](const QString& val)->void {
                onChangedValue(hasFocus()&&!softwareCalling_, val); });
}

//  ***********************************************************************************************
//! @class QcrTabWidget

QcrTabWidget::QcrTabWidget(const QString& _name)
    : QcrControl<int> {*this, _name, 0}
{
    initControl();
    connect(this, &QTabWidget::currentChanged, [this](int val) {
            // To detect user action, we must entirely relie on !softwareCalling,
            // since hasFocus() does not work as we would need.
            onChangedValue(!softwareCalling_, val); });
}

void QcrTabWidget::addTab(QWidget* page, const QString& label)
{
    softwareCalling_ = true;
    QTabWidget::addTab(page, label);
    softwareCalling_ = false;
}

void QcrTabWidget::setTabEnabled(int index, bool on)
    // Needs to be encapsulate because of side effect upon currentIndex.
    // See https://bugreports.qt.io/browse/QTBUG-69930.
{
    softwareCalling_ = true;
    QTabWidget::setTabEnabled(index, on);
    softwareCalling_ = false;
}

bool QcrTabWidget::anyEnabled() const
{
    for (int i=0; i<count(); ++i)
        if (isTabEnabled(i))
            return true;
    return false;
}

void QcrTabWidget::setCurrentIndex(int val)
{
    softwareCalling_ = true;
    QTabWidget::setCurrentIndex(val);
    softwareCalling_ = false;
}
