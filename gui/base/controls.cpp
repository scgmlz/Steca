//  ***********************************************************************************************
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
//  ***********************************************************************************************

#include "controls.h"
#include "gui/base/convert.h"
#include "gui/base/displays.h"
#include "gui/capture_and_replay/cmdexception.h"
#include "gui/capture_and_replay/console.h"
#include <QApplication> // for qApp for new Action
#include <QtGlobal> // to define Q_OS_WIN
#include <iostream> // debug

//  ***********************************************************************************************
//  QAction overloads CTrigger and CToggle
//  ***********************************************************************************************

//! @class CTrigger

CTrigger::CTrigger(const QString& rawname, const QString& text, const QString& iconFile)
    : QAction(text, qApp)
    , CSettable(rawname)
    , tooltip_(text.toLower())
{
    //QAction::setObjectName(name());
    if (iconFile!="")
        setIcon(QIcon(iconFile));
    connect(this, &QAction::triggered, [this]()->void {
            gConsole->log(name()+" trigger"); });
    connect(this, &QAction::changed, [this]()->void {
            QString txt = tooltip_;
            if (!isEnabled())
                txt += "\nThis trigger is currently inoperative.";
            setToolTip(txt); });
    emit changed();
};

CTrigger::CTrigger(
    const QString& name, const QString& text, const QString& iconFile, const QKeySequence& shortcut)
    : CTrigger(name, text, iconFile)
{
    setShortcut(shortcut);
}

void CTrigger::onCommand(const QStringList& args)
{
    if (args[0]!="trigger")
        throw CmdException("Unexpected command");
    trigger();
}

//! @class CToggle

CToggle::CToggle(const QString& rawname, const QString& text, bool on, const QString& iconFile)
    : QAction(text, qApp)
    , CSettable(rawname)
    , tooltip_(text.toLower())
{
    //QAction::setObjectName(CSettable::name());
    if (iconFile!="")
        setIcon(QIcon(iconFile));
    setCheckable(true);
    setChecked(on);
    connect(this, &QAction::toggled, [this](bool val)->void {
            gConsole->log(name()+" switch "+(val ? "on" : "off")); });
    connect(this, &QAction::changed, [this]()->void {
            QString txt = tooltip_;
            if (!isEnabled())
                txt += "\nThis toggle is currently inoperative.";
            else if (isChecked())
                txt += "\nThis toggle is currently checked. Click to uncheck.";
            else
                txt += "\nThis toggle is currently unchecked. Click to check.";
            setToolTip(txt); });
    emit changed();
};

CToggle::CToggle(const QString& name, const QString& text, bool on, const QString& iconFile,
                 const QKeySequence& shortcut)
    : CToggle(name, text, on, iconFile)
{
    setShortcut(shortcut);
}

void CToggle::onCommand(const QStringList& args)
{
    if (args[0]!="switch")
        throw CmdException("Unexpected command");
    if      (args.size()<2)
        throw CmdException("Missing argument to command 'switch'");
    else if (args[1]=="on")
        setChecked(true);
    else if (args[1]=="off")
        setChecked(false);
    else
        throw CmdException("Invalid argument to command 'switch'");
}

//  ***********************************************************************************************
//! @classes with no console connection

XTextButton::XTextButton(QAction* action)
{
    setDefaultAction(action);
    setToolButtonStyle(Qt::ToolButtonTextOnly);
}

XIconButton::XIconButton(QAction* action)
{
    setDefaultAction(action);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
}

//  ***********************************************************************************************
//  owning button classes
//  ***********************************************************************************************

CTextButton::CTextButton(QAction* action)
{
    action->setParent(this);
    setDefaultAction(action);
    setToolButtonStyle(Qt::ToolButtonTextOnly);
}

CIconButton::CIconButton(QAction* action)
{
    action->setParent(this);
    setDefaultAction(action);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
}

//  ***********************************************************************************************
//  control widget classes with console connection
//  ***********************************************************************************************

//! @class CSpinBox
//!
//! A QSpinBox controls an integer value. Therefore normally we need no extra width for a dot.
//! However, sometimes we want to make a QSpinBox exactly as wide as a given QDoubleSpinBox,
//! for nice vertical alignement. Then we use withDot=true.
//!
//! The signal QSpinBox::valueChanged cannot be used to trigger lengthy computations
//! because it will cause duplicate incrementation. A workaround is described at
//! https://forum.qt.io/topic/89011. Here, we explicitly deal with editingFinished and
//! mouse release events.

CSpinBox::CSpinBox(const QString& _name, int ndigits, bool withDot, int min, int max,
                   const QString& tooltip)
    : CSettable(_name)
{
    widgetUtils::setWidth(this, 2+ndigits, withDot);
    setMinimum(min);
    setMaximum(max > min ? max : min);
    if (tooltip!="")
        setToolTip(tooltip);
    reportedValue_ = value();
    connect(this, &QSpinBox::editingFinished, this, &CSpinBox::reportChange);
    connect(this, qOverload<int>(&QSpinBox::valueChanged), [this](int val)->void {
            if(!hasFocus())
                gConsole->log2(false, name()+" set "+QString::number(val)); });
}

void CSpinBox::mouseReleaseEvent(QMouseEvent *event)
{
    QSpinBox::mouseReleaseEvent(event);
    reportChange();
}

void CSpinBox::reportChange()
{
    int val = value();
    if (val == reportedValue_)
        return;
    reportedValue_ = val;
    gConsole->log2(true, name()+" set "+QString::number(val));
    emit valueReleased(val);
}

void CSpinBox::onCommand(const QStringList& args)
{
    if (args[0]!="set")
        throw CmdException("Unexpected command");
    if      (args.size()<2)
        throw CmdException("Missing argument to command 'set'");
    int val = TO_INT(args[1]);
    setValue(val);
    emit valueReleased(val);
}

//! @class CDoubleSpinBox

CDoubleSpinBox::CDoubleSpinBox(const QString& _name, int ndigits, double min, double max)
    : CSettable(_name)
{
    widgetUtils::setWidth(this, 2+ndigits, true);
    if (min>max)
        qSwap(min, max);
    setMinimum(min);
    setMaximum(max);
    reportedValue_ = value();
    connect(this, &QDoubleSpinBox::editingFinished, this, &CDoubleSpinBox::reportChange);
    connect(this, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double val)->void {
            if(!hasFocus())
                gConsole->log2(false, name()+" set "+QString::number(val)); });
}

void CDoubleSpinBox::mouseReleaseEvent(QMouseEvent *event)
{
    QDoubleSpinBox::mouseReleaseEvent(event);
    reportChange();
}

void CDoubleSpinBox::reportChange()
{
    double val = value();
    if (val == reportedValue_)
        return;
    reportedValue_ = val;
    gConsole->log2(true, name()+" set "+QString::number(val));
    emit valueReleased(val);
}

void CDoubleSpinBox::onCommand(const QStringList& args)
{
    if (args[0]!="set")
        throw CmdException("Unexpected command");
    if      (args.size()<2)
        throw CmdException("Missing argument to command 'set'");
    double val = TO_DOUBLE(args[1]);
    setValue(val);
    emit valueReleased(val);
}

//! @class CCheckBox

CCheckBox::CCheckBox(const QString& _name, QAction* action)
    : QCheckBox(action ? action->text().toLower() : "")
    , CSettable(_name)
{
    if (!action)
        return;
    connect(this, &QCheckBox::toggled, [action](bool on) { action->setChecked(on); });
    connect(action, &QAction::toggled, [this](bool on) { setChecked(on); });
    setToolTip(action->toolTip());
    setChecked(action->isChecked());
    connect(this, qOverload<int>(&QCheckBox::stateChanged), [this](int val)->void {
            gConsole->log2(hasFocus(), name()+" set "+QString::number(val)); });
}

CCheckBox::CCheckBox(const QString& name, const QString& text)
    : CCheckBox(name, {})
{
    setText(text);
}

void CCheckBox::onCommand(const QStringList& args)
{
    if (args[0]!="set")
        throw CmdException("Unexpected command");
    if      (args.size()<2)
        throw CmdException("Missing argument to command 'set'");
    setChecked(TO_INT(args[1]));
}

//! @class CRadioButton

CRadioButton::CRadioButton(const QString& _name, const QString& text)
    : QRadioButton(text)
    , CSettable(_name)
{
    connect(this, &QRadioButton::toggled, [this](bool val)->void {
            gConsole->log2(hasFocus(), name()+" switch "+(val?"on":"off")); });
}

void CRadioButton::onCommand(const QStringList& args)
{
    if (args[0]!="switch")
        throw CmdException("Unexpected command");
    if      (args.size()<2)
        throw CmdException("Missing argument to command 'switch'");
    else if (args[1]=="on")
        setChecked(true);
    else if (args[1]=="off")
        setChecked(false);
    else
        throw CmdException("Invalid argument to command 'switch'");
}

//! @class CComboBox

CComboBox::CComboBox(const QString& _name, const QStringList& items)
    : CSettable(_name)
{
    addItems(items);
    connect(this, qOverload<int>(&QComboBox::currentIndexChanged), [this](int val)->void {
            gConsole->log2(hasFocus(), name()+" choose "+QString::number(val)); });
}

void CComboBox::onCommand(const QStringList& args)
{
    if (args[0]!="choose")
        throw CmdException("Unexpected command");
    if (args.size()<2)
        throw CmdException("Missing argument to command 'choose'");
    setCurrentIndex(TO_INT(args[1]));
}

//! @class CTabWidget

CTabWidget::CTabWidget(const QString& _name)
    : CSettable(_name)
{
    connect(this->tabBar(), &QTabBar::tabBarClicked, [this](int val) {
            gConsole->log2(true, name()+" choose "+QString::number(val)); });
    connect(this, &QTabWidget::currentChanged, [this](int val) {
            gConsole->log2(false, name()+" choose "+QString::number(val)); });
}

void CTabWidget::onCommand(const QStringList& args)
{
    if (args[0]!="choose")
        throw CmdException("Unexpected command");
    if (args.size()<2)
        throw CmdException("Missing argument to command 'choose'");
    int val = TO_INT(args[1]);
    if (!isTabEnabled(val))
        throw CmdException("CHosen tab is not enabled");
    setCurrentIndex(val);
}

//! @class CFileDialog

CFileDialog::CFileDialog(QWidget *parent, const QString &caption,
                         const QString &directory, const QString &filter)
    : QFileDialog(parent, caption, directory, filter)
    , CModal("fdia")
    , CSettable("fdia")
{
}

CFileDialog::~CFileDialog()
{
    gConsole->log("fdia select "+selectedFiles().join(';'));
}

int CFileDialog::exec()
{
    if (gConsole->hasCommandsOnStack()) {
        open();
        gConsole->commandsFromStack();
        close();
        return QDialog::Accepted;
    } else
        return QFileDialog::exec();
}

void CFileDialog::onCommand(const QStringList& args)
{
    if        (args[0]=="close") {
        accept();
    } else if (args[0]=="select") {
        if (args.size()<2)
            throw CmdException("Missing argument to command 'select'");
        QStringList list = args[1].split(';');
        QString tmp = '"' + list.join("\" \"") + '"';
        selectFile(tmp);
    } else
        throw CmdException("Unexpected command");
}
