//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/widgets/actions.cpp
//! @brief     Implements enhanced control widgets like QcrSpinBox, QcrRadioButton, and many others
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#include "qcr/widgets/actions.h"
#include "qcr/base/qcrexception.h"
#include "qcr/base/string_ops.h"
#include "qcr/engine/console.h"
//#include "qcr/base/debug.h"
#include <QApplication> // for qApp for new Action
#include <iostream> // debug

#define _SLOT_(Class, method, argType) static_cast<void (Class::*)(argType)>(&Class::method)

//  ***********************************************************************************************
//! @class QcrAction

QcrAction::QcrAction(const QString& text)
    : QAction{text, qApp}
    , tooltip_ {text}
{}

//  ***********************************************************************************************
//! @class QcrTrigger

QcrTrigger::QcrTrigger(const QString& rawname, const QString& text, const QString& iconFile)
    : QcrRegistered{rawname}
    , QcrAction {text}
{
    //QAction::setObjectName(name());
    if (iconFile!="")
        setIcon(QIcon(iconFile));
    QAction::connect(this, &QAction::triggered,
                     [this]()->void {
                         gConsole->log(name());
                         triggerHook_();
                         gRoot->remakeAll(); });
    QAction::connect(this, &QAction::changed,
                     [this]()->void {
                         QString txt = tooltip_;
                         if (!isEnabled())
                             txt += "\nThis trigger is currently inoperative.";
                         setToolTip(txt); });
}

QcrTrigger::QcrTrigger(
    const QString& name, const QString& text, const QString& iconFile, const QKeySequence& shortcut)
    : QcrTrigger{name, text, iconFile}
{
    setShortcut(shortcut);
}

void QcrTrigger::setFromCommand(const QString& arg)
{
    if (arg!="")
        throw QcrException{"Found unexpected argument to trigger command"};
    if (!isEnabled()) {
        qDebug() << "Ignoring command" << arg << "because trigger" << name() << "is not enabled.";
        return;
    }
    trigger();
}

//  ***********************************************************************************************
//! @class QcrTextTriggerButton

QcrTextTriggerButton::QcrTextTriggerButton(QcrTrigger* trigger, bool ownsTrigger)
    : QcrBase {trigger->name()+"Btn"}
    , trigger_{trigger}
    , ownsTrigger_ {ownsTrigger}
{
    setDefaultAction(trigger);
    setToolButtonStyle(Qt::ToolButtonTextOnly);
    setRemake([=](){trigger->remake();});
}

QcrTextTriggerButton::QcrTextTriggerButton(QcrTrigger* trigger)
    : QcrTextTriggerButton{trigger, false}
{}

QcrTextTriggerButton::QcrTextTriggerButton(const QString& name, const QString& text)
    : QcrTextTriggerButton{new QcrTrigger{name, text}, true}
{}

//! Destructs this QcrTextTriggerButton. Also destructs the trigger, if it owns one.
QcrTextTriggerButton::~QcrTextTriggerButton()
{
    if (ownsTrigger_)
        delete trigger_;
}

//  ***********************************************************************************************
//! @class QcrIconTriggerButton

QcrIconTriggerButton::QcrIconTriggerButton(QcrTrigger* trigger, bool ownsTrigger)
    : QcrBase {trigger->name()+"Btn"}
    , trigger_{trigger}
    , ownsTrigger_ {ownsTrigger}
{
    setDefaultAction(trigger);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setRemake([=](){trigger->remake();});
}

QcrIconTriggerButton::QcrIconTriggerButton(QcrTrigger* trigger)
    : QcrIconTriggerButton{trigger, false}
{}

QcrIconTriggerButton::QcrIconTriggerButton(
    const QString& name, const QString& text, const QString& iconFile)
    : QcrIconTriggerButton{new QcrTrigger{name, text, iconFile}, true}
{}

//! Destructs this QcrIconTriggerButton. Also destructs the trigger, if it owns one.
QcrIconTriggerButton::~QcrIconTriggerButton()
{
    if (ownsTrigger_)
        delete trigger_;
}

//  ***********************************************************************************************
//! @class QcrToggle

QcrToggle::QcrToggle(const QString& rawname, const QString& text, bool on,
                     const QString& iconFile, const QKeySequence& shortcut)
    : QcrSingleValue<bool> {rawname, on}
    , QcrAction{text}
{
    initToggle(iconFile, shortcut);
}

QcrToggle::QcrToggle(const QString& rawname, QcrCell<bool>* cell, const QString& text,
                     const QString& iconFile, const QKeySequence& shortcut)
    : QcrSingleValue<bool> {rawname, cell}
    , QcrAction{text}
{
    initToggle(iconFile, shortcut);
}

void QcrToggle::initToggle(const QString& iconFile, const QKeySequence& shortcut)
{
    setShortcut(shortcut);
    setCheckable(true);
    //QAction::setObjectName(QcrRegistered::name());
    if (iconFile!="")
        setIcon(QIcon(iconFile));
    doSetValue(cell_->val());
    QAction::connect(this, &QAction::toggled, [this](bool val){
            //qDebug()<<"TOGGLE "<<name()<<"toggled";
            onChangedValue(val);});
    QAction::connect(this, &QAction::changed, [this]()->void {
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
}

//  ***********************************************************************************************
//! @class QcrTextToggleButton

//! Generic private constructor that is called by the public constructors.
QcrTextToggleButton::QcrTextToggleButton(QcrToggle* toggle, bool ownsToggle)
    : QcrBase {toggle->name()+"Btn"}
    , toggle_{toggle}
    , ownsToggle_ {ownsToggle}
{
    setDefaultAction(toggle);
    setToolButtonStyle(Qt::ToolButtonTextOnly);
    setRemake([=](){toggle->remake();});
}

//! Constructs QcrTextToggleButton that is connected with external QcrToggle.
QcrTextToggleButton::QcrTextToggleButton(QcrToggle* toggle)
    : QcrTextToggleButton{toggle, false}
{}

//! Constructs QcrTextToggleButton and a QcrToggle owned by the former.
QcrTextToggleButton::QcrTextToggleButton(const QString& name, const QString& text, bool on)
    : QcrTextToggleButton{new QcrToggle{name, text, on}, true}
{}

//! Destructs this QcrTextToggleButton. Also destructs the toggle, if it owns one.
QcrTextToggleButton::~QcrTextToggleButton()
{
    if (ownsToggle_)
        delete toggle_;
}


//  ***********************************************************************************************
//! @class QcrIconToggleButton

QcrIconToggleButton::QcrIconToggleButton(QcrToggle* toggle, bool ownsToggle)
    : QcrBase {toggle->name()+"Btn"}
    , toggle_{toggle}
    , ownsToggle_ {ownsToggle}
{
    setDefaultAction(toggle);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setRemake([=](){toggle->remake();});
}

QcrIconToggleButton::QcrIconToggleButton(QcrToggle* toggle)
    : QcrIconToggleButton{toggle, false}
{}

QcrIconToggleButton::QcrIconToggleButton(
    const QString& name, const QString& text, bool on,
    const QString& iconFile, const QKeySequence& shortcut)
    : QcrIconToggleButton{new QcrToggle{name, text, on, iconFile, shortcut}, true}
{}

QcrIconToggleButton::QcrIconToggleButton(
    const QString& name, QcrCell<bool>* cell, const QString& text,
    const QString& iconFile, const QKeySequence& shortcut)
    : QcrIconToggleButton{new QcrToggle{name, cell, text, iconFile, shortcut}, true}
{}

QcrIconToggleButton::~QcrIconToggleButton()
{
    if (ownsToggle_)
        delete toggle_;
}
