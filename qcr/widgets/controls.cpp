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
#include "qcr/base/debug.h"
#include "qcr/base/qcrexception.h"
#include "qcr/engine/console.h"
#include "qcr/base/string_ops.h"
#include "qcr/widgets/displays.h"
#include <QApplication> // for qApp for new Action
#include <iostream> // debug

#define _SLOT_(Class, method, argType) static_cast<void (Class::*)(argType)>(&Class::method)

//  Action classes

//  ***********************************************************************************************
//! @class QcrAction

QcrAction::QcrAction(const QString& text)
    : QAction(text, qApp)
    , tooltip_(text.toLower())
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
//! @class QcrToggle

QcrToggle::QcrToggle(const QString& rawname, const QString& text, bool on,
                     const QString& iconFile, const QKeySequence& shortcut)
    : QcrToggle {rawname, nullptr, text, iconFile, shortcut}
{
    programaticallySetValue(on);
}

QcrToggle::QcrToggle(const QString& rawname, SingleValueCell<bool>* cell, const QString& text,
                     const QString& iconFile, const QKeySequence& shortcut)
    : QcrAction {text}
    , QcrControl<bool> {*this, rawname, cell}
{
    setShortcut(shortcut);
    init();
    //QAction::setObjectName(QcrSettable::name());
    if (iconFile!="")
        setIcon(QIcon(iconFile));
    setCheckable(true);
    if (cell)
        doSetValue(cell->val());
    connect(this, &QAction::toggled, this, [this](bool val){
            //qDebug()<<"TOGGLE "<<name()<<"toggled";
            onChangedValue(hasFocus(), val);});
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
//! @classes with no console connection

QcrTextButton::QcrTextButton(QcrAction* action)
{
    setDefaultAction(action);
    setToolButtonStyle(Qt::ToolButtonTextOnly);
}

QcrIconButton::QcrIconButton(QcrAction* action)
{
    setDefaultAction(action);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
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

QcrSpinBox::QcrSpinBox(const QString& _name, int ndigits,
                       bool withDot, int min, int max, const QString& tooltip)
    : QcrSpinBox(_name, nullptr, ndigits, withDot, min, max, tooltip)
{}

QcrSpinBox::QcrSpinBox(const QString& _name, SingleValueCell<int>* cell, int ndigits,
                       bool withDot, int min, int max, const QString& tooltip)
    : QcrControl<int> {*this, _name, cell}
{
    strOp::setWidth(this, 2+ndigits, withDot);
    ASSERT(min<=max);
    setMinimum(min);
    setMaximum(max);
    if (cell)
        doSetValue(cell->val());
    init();
    if (tooltip!="")
        setToolTip(tooltip);
    reportedValue_ = value();
    connect(this, &QSpinBox::editingFinished, this, &QcrSpinBox::reportChange);
    connect(this, _SLOT_(QSpinBox,valueChanged,int), [this](int val)->void {
            if(!hasFocus())
                onChangedValue(hasFocus(), val); });
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
    reportedValue_ = val;
    onChangedValue(hasFocus(), val);
    emit valueReleased(val);
}

void QcrSpinBox::executeConsoleCommand(const QString& arg)
{
    int val = strOp::to_i(arg);
    programaticallySetValue(val);
    emit valueReleased(val);
}

//  ***********************************************************************************************
//! @class QcrDoubleSpinBox

QcrDoubleSpinBox::QcrDoubleSpinBox(const QString& _name, int ndigits, double min, double max)
    : QcrDoubleSpinBox(_name, nullptr, ndigits, min, max)
{}

QcrDoubleSpinBox::QcrDoubleSpinBox(
    const QString& _name, SingleValueCell<double>* cell, int ndigits, double min, double max)
    : QcrControl<double> {*this, _name, cell}
{
    strOp::setWidth(this, 2+ndigits, true);
    setDecimals(ndigits);
    ASSERT(min<=max);
    setMinimum(min);
    setMaximum(max);
    if (cell)
        doSetValue(cell->val());
    init();
    reportedValue_ = value();
    connect(this, &QDoubleSpinBox::editingFinished, this, &QcrDoubleSpinBox::reportChange);
    connect(this, _SLOT_(QDoubleSpinBox,valueChanged,double), [this](double val)->void {
            if(!hasFocus())
                onChangedValue(hasFocus(), val); });
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
    reportedValue_ = val;
    onChangedValue(hasFocus(), val);
    emit valueReleased(val);
}

void QcrDoubleSpinBox::executeConsoleCommand(const QString& arg)
{
    double val = strOp::to_d(arg);
    programaticallySetValue(val);
    emit valueReleased(val);
}

//  ***********************************************************************************************
//! @class QcrCheckBox

QcrCheckBox::QcrCheckBox(const QString& _name, const QString& text, bool val)
    : QCheckBox {text}
    , QcrControl<bool> {*this, _name}
{
    doSetValue(val);
    init();
    connect(this, _SLOT_(QCheckBox,stateChanged,int), [this](int val)->void {
            onChangedValue(hasFocus(), (bool)val); });
}

QcrCheckBox::QcrCheckBox(const QString& _name, const QString& text, SingleValueCell<bool>* cell)
    : QCheckBox {text}
    , QcrControl<bool> {*this, _name, cell}
{
    doSetValue(cell->val());
    init();
    connect(this, _SLOT_(QCheckBox,stateChanged,int), [this](int val)->void {
            onChangedValue(hasFocus(), (bool)val); });
}

//  ***********************************************************************************************
//! @class QcrRadioButton

QcrRadioButton::QcrRadioButton(const QString& _name, const QString& text)
    : QRadioButton {text}
    , QcrControl<bool> {*this, _name}
{
    init();
    connect(this, &QRadioButton::toggled, [this](bool val)->void {
            onChangedValue(hasFocus(), val); });
}

//  ***********************************************************************************************
//! @class QcrComboBox

QcrComboBox::QcrComboBox(const QString& _name, const QStringList& items)
    : QcrControl<int> {*this, _name}
{
    init();
    addItems(items);
    connect(this, _SLOT_(QComboBox,currentIndexChanged,int), [this](int val)->void {
            onChangedValue(hasFocus(), val); });
}

void QcrComboBox::addItems(const QStringList& texts)
{
    softwareCalling_ = true;
    QComboBox::addItems(texts);
    softwareCalling_ = false;
}

//  ***********************************************************************************************
//! @class QcrLineEdit

QcrLineEdit::QcrLineEdit(const QString& _name, const QString& val)
    : QcrControl<QString> {*this, _name}
{
    init();
    // For unknown reason, hasFocus() is not always false when setText is called programmatically;
    // therefore we must use another criterion to distinuish user actions from other calls.
    // The following works, but has the drawback that a user action is logged not only as such,
    // but also in a second line as if there were an indirect call.
    connect(this, _SLOT_(QLineEdit,textEdited,const QString&),
            [this](const QString& val)->void {
                onChangedValue(hasFocus(), val); });
    connect(this, _SLOT_(QLineEdit,textChanged,const QString&),
            [this](const QString& val)->void {
                onChangedValue(hasFocus(), val); });
    programaticallySetValue(val);
}

//  ***********************************************************************************************
//! @class QcrTabWidget

QcrTabWidget::QcrTabWidget(const QString& _name)
    : QcrControl<int> {*this, _name}
    , defaultCell {_name, 0}
{
    cell_ = &defaultCell;
    init();
    connect(this->tabBar(), &QTabBar::tabBarClicked, [this](int val) {
            qDebug() << "tabBarClicked " << val; });
    connect(this, &QTabWidget::currentChanged, [this](int val) {
            qDebug() << "tabBarChanged " << val;
            if (!isTabEnabled(val))
                qFatal("Chosen tab is not enabled");
            onChangedValue(hasFocus(), val); });
}

void QcrTabWidget::addTab(QWidget* page, const QString& label)
{
    softwareCalling_ = true;
    QTabWidget::addTab(page, label);
    softwareCalling_ = false;
}

void QcrTabWidget::setCurrentIndex(int val)
{
    QTabWidget::setCurrentIndex(val);
}

//  ***********************************************************************************************
//! @class QcrDialog

QcrDialog::QcrDialog(QWidget* parent, const QString& caption)
    : QDialog {parent}
    , QcrModal {*this, "dlog"}
{
    setWindowTitle(caption);
}

QcrDialog::~QcrDialog()
{
    gConsole->log("dlog closing");
}

int QcrDialog::exec()
{
    if (gConsole->hasCommandsOnStack()) {
        open();
        gConsole->commandsFromStack();
        close();
        return QDialog::Accepted;
    } else
        return QDialog::exec();
}

void QcrDialog::executeConsoleCommand(const QString& arg)
{
    if (arg=="")
        throw QcrException("Empty argument in Dialog command");
    if (arg=="close") {
        accept();
        return;
    }
}

//  ***********************************************************************************************
//! @class QcrFileDialog

QcrFileDialog::QcrFileDialog(
    QWidget* parent, const QString& caption, const QString& directory, const QString& filter)
    : QFileDialog {parent, caption, directory, filter}
    , QcrModal {*this, "fdia"}
{}

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
        return QDialog::exec();
}

void QcrFileDialog::executeConsoleCommand(const QString& arg)
{
    if (arg=="")
        throw QcrException("Empty argument in FileDialog command");
    if (arg=="close") {
        accept();
        return;
    }
    QStringList args = arg.split(' ');
    if (args[0]!="select")
        throw QcrException("Unexpected filedialog command");
    if (args.size()<2)
        throw QcrException("Missing argument to command 'select'");
    QStringList list = args[1].split(';');
    QString tmp = '"' + list.join("\" \"") + '"';
    selectFile(tmp);
}
