//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/widgets/controls.cpp
//! @brief     Implements functions that return new Qt objects
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#include "controls.h"
#include "qcr/engine/debug.h"
#include "qcr/engine/qcrexception.h"
#include "qcr/engine/console.h"
#include "qcr/widgets/convert.h"
#include "qcr/widgets/displays.h"
#include <QApplication> // for qApp for new Action
#include <iostream> // debug

#define _SLOT_(Class, method, argType) static_cast<void (Class::*)(argType)>(&Class::method)

//  ***********************************************************************************************
//  QAction overloads QcrTrigger and QcrToggle
//  ***********************************************************************************************

//! @class QcrTrigger

QcrTrigger::QcrTrigger(const QString& rawname, const QString& text, const QString& iconFile)
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
    EMITS(("Trigger "+name()),changed());
};

QcrTrigger::QcrTrigger(
    const QString& name, const QString& text, const QString& iconFile, const QKeySequence& shortcut)
    : QcrTrigger(name, text, iconFile)
{
    setShortcut(shortcut);
}

void QcrTrigger::onCommand(const QStringList& args)
{
    if (args[0]!="trigger")
        throw QcrException("Unexpected trigger command");
    trigger();
}

//! @class QcrToggle

QcrToggle::QcrToggle(const QString& rawname, const QString& text, bool on, const QString& iconFile)
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
    EMITS(("Toggle "+name()),changed());
};

QcrToggle::QcrToggle(const QString& name, const QString& text, bool on, const QString& iconFile,
                 const QKeySequence& shortcut)
    : QcrToggle(name, text, on, iconFile)
{
    setShortcut(shortcut);
}

void QcrToggle::onCommand(const QStringList& args)
{
    if (args[0]!="switch")
        throw QcrException("Unexpected toggle command");
    if      (args.size()<2)
        throw QcrException("Missing argument to command 'switch'");
    else if (args[1]=="on")
        setChecked(true);
    else if (args[1]=="off")
        setChecked(false);
    else
        throw QcrException("Invalid argument to command 'switch'");
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

QcrTextButton::QcrTextButton(QAction* action)
{
    action->setParent(this);
    setDefaultAction(action);
    setToolButtonStyle(Qt::ToolButtonTextOnly);
}

QcrIconButton::QcrIconButton(QAction* action)
{
    action->setParent(this);
    setDefaultAction(action);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
}

//  ***********************************************************************************************
//  control widget classes with console connection
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

QcrSpinBox::QcrSpinBox(const QString& _name, int ndigits, bool withDot, int min, int max,
                   const QString& tooltip)
    : CSettable(_name)
{
    widgetUtils::setWidth(this, 2+ndigits, withDot);
    setMinimum(min);
    setMaximum(max > min ? max : min);
    if (tooltip!="")
        setToolTip(tooltip);
    reportedValue_ = value();
    connect(this, &QSpinBox::editingFinished, this, &QcrSpinBox::reportChange);
    connect(this, _SLOT_(QSpinBox,valueChanged,int), [this](int val)->void {
            if(!hasFocus())
                gConsole->log2(false, name()+" set "+QString::number(val)); });
}

void QcrSpinBox::mouseReleaseEvent(QMouseEvent *event)
{
    QSpinBox::mouseReleaseEvent(event);
    reportChange();
}

void QcrSpinBox::reportChange()
{
    int val = value();
    if (val == reportedValue_)
        return;
    reportedValue_ = val;
    gConsole->log2(true, name()+" set "+QString::number(val));
    EMITS("QcrSpinBox::reportChange", valueReleased(val));
}

void QcrSpinBox::onCommand(const QStringList& args)
{
    if (args[0]!="set")
        throw QcrException("Unexpected SpinBox command");
    if      (args.size()<2)
        throw QcrException("Missing argument to command 'set'");
    int val = TO_INT(args[1]);
    setValue(val);
    EMITS("QcrSpinBox::onCommand", valueReleased(val));
}

//! @class QcrDoubleSpinBox

QcrDoubleSpinBox::QcrDoubleSpinBox(const QString& _name, int ndigits, double min, double max)
    : CSettable(_name)
{
    widgetUtils::setWidth(this, 2+ndigits, true);
    if (min>max)
        qSwap(min, max);
    setMinimum(min);
    setMaximum(max);
    reportedValue_ = value();
    connect(this, &QDoubleSpinBox::editingFinished, this, &QcrDoubleSpinBox::reportChange);
    connect(this, _SLOT_(QDoubleSpinBox,valueChanged,double), [this](double val)->void {
            if(!hasFocus())
                gConsole->log2(false, name()+" set "+QString::number(val)); });
}

void QcrDoubleSpinBox::mouseReleaseEvent(QMouseEvent *event)
{
    QDoubleSpinBox::mouseReleaseEvent(event);
    reportChange();
}

void QcrDoubleSpinBox::reportChange()
{
    double val = value();
    if (val == reportedValue_)
        return;
    reportedValue_ = val;
    gConsole->log2(true, name()+" set "+QString::number(val));
    EMITS("QcrDoubleSpinBox::reportChange", valueReleased(val));
}

void QcrDoubleSpinBox::onCommand(const QStringList& args)
{
    if (args[0]!="set")
        throw QcrException("Unexpected DoubleSpinBox command");
    if      (args.size()<2)
        throw QcrException("Missing argument to command 'set'");
    double val = TO_DOUBLE(args[1]);
    setValue(val);
    EMITS("QcrDoubleSpinBox::onCommand", valueReleased(val));
}

//! @class QcrCheckBox

QcrCheckBox::QcrCheckBox(const QString& _name, const QString& text)
    : QCheckBox(text)
    , CSettable(_name)
{
    connect(this, _SLOT_(QCheckBox,stateChanged,int), [this](int val)->void {
            gConsole->log2(hasFocus(), name()+" set "+QString::number(val)); });
}

void QcrCheckBox::onCommand(const QStringList& args)
{
    if (args[0]!="set")
        throw QcrException("Unexpected CheckBox command");
    if      (args.size()<2)
        throw QcrException("Missing argument to command 'set'");
    setChecked(TO_INT(args[1]));
}

//! @class QcrRadioButton

QcrRadioButton::QcrRadioButton(const QString& _name, const QString& text)
    : QRadioButton(text)
    , CSettable(_name)
{
    connect(this, &QRadioButton::toggled, [this](bool val)->void {
            gConsole->log2(hasFocus(), name()+" switch "+(val?"on":"off")); });
}

void QcrRadioButton::onCommand(const QStringList& args)
{
    if (args[0]!="switch")
        throw QcrException("Unexpected RadioButton command");
    if      (args.size()<2)
        throw QcrException("Missing argument to command 'switch'");
    else if (args[1]=="on")
        setChecked(true);
    else if (args[1]=="off")
        setChecked(false);
    else
        throw QcrException("Invalid argument to command 'switch'");
}

//! @class QcrComboBox

QcrComboBox::QcrComboBox(const QString& _name, const QStringList& items)
    : CSettable(_name)
{
    addItems(items);
    connect(this, _SLOT_(QComboBox,currentIndexChanged,int), [this](int val)->void {
            gConsole->log2(hasFocus(), name()+" choose "+QString::number(val)); });
}

void QcrComboBox::onCommand(const QStringList& args)
{
    if (args[0]!="choose")
        throw QcrException("Unexpected ComboBox command");
    if (args.size()<2)
        throw QcrException("Missing argument to command 'choose'");
    setCurrentIndex(TO_INT(args[1]));
}

//! @class QcrLineEdit

QcrLineEdit::QcrLineEdit(const QString& _name, const QString& val)
    : CSettable(_name)
{
    // For unknown reason, hasFocus() is not always false when setText is called programmatically;
    // therefore we must use another criterion to distinuish user actions from other calls.
    // The following works, but has the drawback that a user action is logged not only as such,
    // but also in a second line as if there were an indirect call.
    connect(this, _SLOT_(QLineEdit,textEdited,const QString&),
            [this](const QString& val)->void {
                gConsole->log2(true, name()+" settext "+val); });
    connect(this, _SLOT_(QLineEdit,textChanged,const QString&),
            [this](const QString& val)->void {
                gConsole->log2(false, name()+" settext "+val); });
    setText(val);
}

void QcrLineEdit::onCommand(const QStringList& args)
{
    if (args[0]!="settext")
        throw QcrException("Unexpected LineEdit command");
    if (args.size()<2)
        throw QcrException("Missing argument to command 'settext'");
    setText(args[1]); // TODO handle text that contains blanks
}

//! @class QcrTabWidget

QcrTabWidget::QcrTabWidget(const QString& _name)
    : CSettable(_name)
{
    connect(this->tabBar(), &QTabBar::tabBarClicked, [this](int val) {
            gConsole->log2(true, name()+" choose "+QString::number(val)); });
    connect(this, &QTabWidget::currentChanged, [this](int val) {
            gConsole->log2(false, name()+" choose "+QString::number(val)); });
}

void QcrTabWidget::onCommand(const QStringList& args)
{
    if (args[0]!="choose")
        throw QcrException("Unexpected tabwidget command");
    if (args.size()<2)
        throw QcrException("Missing argument to command 'choose'");
    int val = TO_INT(args[1]);
    if (!isTabEnabled(val))
        throw QcrException("Chosen tab is not enabled");
    setCurrentIndex(val);
}

//! @class QcrFileDialog

QcrFileDialog::QcrFileDialog(QWidget *parent, const QString &caption,
                         const QString &directory, const QString &filter)
    : QFileDialog(parent, caption, directory, filter)
    , CModal("fdia")
    , CSettable("fdia")
{
}

QcrFileDialog::~QcrFileDialog()
{
    gConsole->log("fdia select "+selectedFiles().join(';'));
}

int QcrFileDialog::exec()
{
    if (gConsole->hasCommandsOnStack()) {
        open();
        gConsole->commandsFromStack();
        close();
        return QDialog::Accepted;
    } else
        return QFileDialog::exec();
}

void QcrFileDialog::onCommand(const QStringList& args)
{
    if        (args[0]=="close") {
        accept();
    } else if (args[0]=="select") {
        if (args.size()<2)
            throw QcrException("Missing argument to command 'select'");
        QStringList list = args[1].split(';');
        QString tmp = '"' + list.join("\" \"") + '"';
        selectFile(tmp);
    } else
        throw QcrException("Unexpected filedialog command");
}
