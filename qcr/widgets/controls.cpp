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
#include "qcr/base/qcrexception.h"
#include "qcr/base/string_ops.h"
#include "qcr/engine/console.h"
//#include "qcr/base/debug.h"
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
            gConsole->log(name());
            triggerHook_();
            gRoot->remakeAll(); });
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
    if (!isEnabled()) {
        qDebug() << "Ignoring command" << arg << "because trigger" << name() << "is not enabled.";
        return;
    }
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
    doSetValue(cell_->val());
    connect(this, &QAction::toggled, this, [this](bool val){
            //qDebug()<<"TOGGLE "<<name()<<"toggled";
            onChangedValue(val);});
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
    ASSERT(min<=cell_->val() && cell_->val()<=max);
    doSetValue(cell_->val());
    connect(this, &QSpinBox::editingFinished, this, [this]() {
            onChangedValue(value()); });
    connect(this, _SLOT_(QSpinBox,valueChanged,int), [this](int val)->void {
            if(!hasFocus())
                onChangedValue(val); });
}

void QcrSpinBox::mouseReleaseEvent(QMouseEvent* event)
{
    QSpinBox::mouseReleaseEvent(event);
    onChangedValue(value());
}

void QcrSpinBox::executeConsoleCommand(const QString& arg)
{
    int val = strOp::to_i(arg);
    doSetValue(val);
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
    ASSERT(min<=cell_->val() && cell_->val()<=max);
    doSetValue(cell_->val());
    connect(this, &QDoubleSpinBox::editingFinished, this, [this]() {
            onChangedValue(value()); });
    connect(this, _SLOT_(QDoubleSpinBox,valueChanged,double), [this](double val)->void {
            if(!hasFocus())
                onChangedValue(val); });
}

void QcrDoubleSpinBox::mouseReleaseEvent(QMouseEvent* event)
{
    QDoubleSpinBox::mouseReleaseEvent(event);
    onChangedValue(value());
}

void QcrDoubleSpinBox::executeConsoleCommand(const QString& arg)
{
    double val = strOp::to_d(arg);
    doSetValue(val);
}

//  ***********************************************************************************************
//! @class QcrCheckBox

QcrCheckBox::QcrCheckBox(const QString& _name, const QString& text, QcrCell<bool>* cell)
    : QCheckBox {text}
    , QcrControl<bool> {*this, _name, cell}
{
    doSetValue(cell_->val());
    connect(this, _SLOT_(QCheckBox,stateChanged,int), [this](int val)->void {
            onChangedValue((bool)val); });
}

//  ***********************************************************************************************
//! @class QcrRadioButton

QcrRadioButton::QcrRadioButton(const QString& _name, const QString& text, bool val)
    : QRadioButton {text}
    , QcrControl<bool> {*this, _name, val}
{
    doSetValue(cell_->val());
    setAutoExclusive(false); // TODO provide int-valued Qcr wrapper for exclusive radio buttons
    connect(this, _SLOT_(QRadioButton,toggled,bool), [this,_name](bool val)->void {
            onChangedValue(val); });
}

QcrRadioButton::QcrRadioButton(const QString& _name, const QString& text, QcrCell<bool>* cell)
    : QRadioButton {text}
    , QcrControl<bool> {*this, _name, cell}
{
    doSetValue(cell_->val());
    setAutoExclusive(false);
    connect(this, _SLOT_(QRadioButton,toggled,bool), [this](bool val)->void {
            onChangedValue(val); });
}

//  ***********************************************************************************************
//! @class QcrComboBox

QcrComboBox::QcrComboBox(
    const QString& _name, QcrCell<int>* _cell, std::function<QStringList()> _makeTags)
    : QcrControl<int> {*this, _name, _cell}
    , makeTags_(_makeTags)
{
    tags_ = makeTags_();
    QComboBox::addItems(tags_);
    doSetValue(cell_->val());
    connect(this, _SLOT_(QComboBox,currentIndexChanged,int), [this](int val)->void {
            if (!spuriousCall_)
                onChangedValue(val); });
}

void QcrComboBox::remake()
{
    if (isVisible()) {
        QStringList newTags = makeTags_();
        if (newTags!=tags_) { // clear&addItems only when needed, to avoid changes of currentIndex
            const int oldIdx = currentIndex();
            spuriousCall_ = true;
            QComboBox::clear();
            QComboBox::addItems(newTags);
            QComboBox::setCurrentIndex(0<=oldIdx&&oldIdx<count()?oldIdx:0);
            spuriousCall_ = false;
            tags_ = newTags;
        }
    }
    QcrMixin::remake();
}

//  ***********************************************************************************************
//! @class QcrLineEdit

QcrLineEdit::QcrLineEdit(const QString& _name, const QString& val)
    : QcrControl<QString> {*this, _name, val}
{
    doSetValue(cell_->val());
    // For unknown reason, hasFocus() is not always false when setText is called programmatically;
    // therefore we must use another criterion to distinuish user actions from other calls.
    // The following works, but has the drawback that a user action is logged not only as such,
    // but also in a second line as if there were an indirect call.
    connect(this, _SLOT_(QLineEdit,textEdited,const QString&),
            [this](const QString& val)->void {
                onChangedValue(val); });
    connect(this, _SLOT_(QLineEdit,textChanged,const QString&),
            [this](const QString& val)->void {
        onChangedValue(val); });
}

void QcrLineEdit::doSetValue(QString val)
{
    //keep cursor pos, so the cursor doesn't always jump to the end:
    int oldCursorPos = cursorPosition();
    setText(val);
    this->setCursorPosition(oldCursorPos);
}

//  ***********************************************************************************************
//! @class QcrTabWidget

QcrTabWidget::QcrTabWidget(const QString& _name)
    : QcrControl<int> {*this, _name, 0}
{
    doSetValue(cell_->val());
    connect(this, &QTabWidget::currentChanged, [this](int val) {
            if (spuriousCall_)
                return;
            onChangedValue(val); });
}

void QcrTabWidget::addTab(QWidget* page, const QString& label)
{
    spuriousCall_ = true;
    QTabWidget::addTab(page, label);
    spuriousCall_ = false;
}

void QcrTabWidget::setTabEnabled(int index, bool on)
    // Needs to be encapsulate because of side effect upon currentIndex.
    // See https://bugreports.qt.io/browse/QTBUG-69930.
{
    spuriousCall_ = true;
    QTabWidget::setTabEnabled(index, on);
    spuriousCall_ = false;
}

void QcrTabWidget::setCurrentIndex(int val)
{
    QTabWidget::setCurrentIndex(val);
}

bool QcrTabWidget::anyEnabled() const
{
    for (int i=0; i<count(); ++i)
        if (isTabEnabled(i))
            return true;
    return false;
}
