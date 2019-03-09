//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/widgets/controls.cpp
//! @brief     Implements enhanced control widgets like QcrSpinBox, QcrRadioButton, and many others
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
    : QcrSingleValue<int> {_name, cell}
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
    QSpinBox::connect(this, &QSpinBox::editingFinished, [this]() {
            onChangedValue(value()); });
    QSpinBox::connect(this, _SLOT_(QSpinBox,valueChanged,int), [this](int val)->void {
            if(!hasFocus())
                onChangedValue(val); });
}

void QcrSpinBox::mouseReleaseEvent(QMouseEvent* event)
{
    QSpinBox::mouseReleaseEvent(event);
    onChangedValue(value());
}

void QcrSpinBox::setFromCommand(const QString& arg)
{
    int val = strOp::to_i(arg);
    doSetValue(val);
}

//  ***********************************************************************************************
//! @class QcrDoubleSpinBox

QcrDoubleSpinBox::QcrDoubleSpinBox(
    const QString& _name, QcrCell<double>* cell, int nDigits, int nDecimals,
    double min, double max, const QString& tooltip)
    : QcrSingleValue<double> {_name, cell}
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
    QSpinBox::connect(this, &QDoubleSpinBox::editingFinished, [this]() {
            onChangedValue(value()); });
    QSpinBox::connect(this, _SLOT_(QDoubleSpinBox,valueChanged,double), [this](double val)->void {
            if(!hasFocus())
                onChangedValue(val); });
}

void QcrDoubleSpinBox::mouseReleaseEvent(QMouseEvent* event)
{
    QDoubleSpinBox::mouseReleaseEvent(event);
    onChangedValue(value());
}

void QcrDoubleSpinBox::setFromCommand(const QString& arg)
{
    double val = strOp::to_d(arg);
    doSetValue(val);
}

//  ***********************************************************************************************
//! @class QcrCheckBox

QcrCheckBox::QcrCheckBox(const QString& _name, const QString& text, QcrCell<bool>* cell)
    : QcrSingleValue<bool> {_name, cell}
    , QCheckBox{text}
{
    doSetValue(cell_->val());
    QCheckBox::connect(this, _SLOT_(QCheckBox,stateChanged,int), [this](int val)->void {
            onChangedValue((bool)val); });
}

//  ***********************************************************************************************
//! @class QcrRadioButton

QcrRadioButton::QcrRadioButton(const QString& _name, const QString& text, bool val)
    : QcrSingleValue<bool> {_name, val}
    , QRadioButton{text}
{
    doSetValue(cell_->val());
    setAutoExclusive(false); // TODO provide int-valued Qcr wrapper for exclusive radio buttons
    QRadioButton::connect(this, _SLOT_(QRadioButton,toggled,bool), [this,_name](bool val)->void {
            onChangedValue(val); });
}

QcrRadioButton::QcrRadioButton(const QString& _name, const QString& text, QcrCell<bool>* cell)
    : QcrSingleValue<bool> {_name, cell}
    , QRadioButton{text}
{
    doSetValue(cell_->val());
    setAutoExclusive(false);
    QRadioButton::connect(this, _SLOT_(QRadioButton,toggled,bool), [this](bool val)->void {
            onChangedValue(val); });
}

//  ***********************************************************************************************
//! @class QcrComboBox

QcrComboBox::QcrComboBox(
    const QString& _name, QcrCell<int>* _cell,
    const bool _haveRemakeTagsFunction, const QStringList& _tags,
    const std::function<QStringList()> _makeTags)
    : QcrSingleValue<int>{_name, _cell}
    , haveRemakeTagsFunction_{_haveRemakeTagsFunction}
    , tags_{_tags}
    , makeTags_{_makeTags}
{
    QComboBox::addItems(tags_);
    doSetValue(cell_->val());
    QComboBox::connect(this, _SLOT_(QComboBox,currentIndexChanged,int), [this](int val)->void {
            if (!spuriousCall_)
                onChangedValue(val, itemText(val)); });
}

QcrComboBox::QcrComboBox(
    const QString& _name, QcrCell<int>* _cell, const std::function<QStringList()> _makeTags)
    : QcrComboBox{_name, _cell, true, _makeTags(), _makeTags}
{
}

QcrComboBox::QcrComboBox(
    const QString& _name, QcrCell<int>* _cell, const QStringList& _tags)
    : QcrComboBox{_name, _cell, false, _tags, [](){return QStringList{};}}
{
}

void QcrComboBox::remake()
{
    if (haveRemakeTagsFunction_ && isVisible()) {
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
    QcrBase::remake();
}

//  ***********************************************************************************************
//! @class QcrRadioBox

QcrRadioBox::QcrRadioBox(
    const QString& _name, const QString& _headline, QcrCell<int>* _cell,
    const QStringList& _tags, QLayout* _layout)
    : QcrSingleValue<int> {_name, _cell}
    , tags_{_tags}
{
    ASSERT(size()>0);
    for (int i=0; i<size(); ++i) {
        auto* rb = new QRadioButton{_tags[i]};
        buttons_.push_back(rb);
        rb->setAutoExclusive(true);
        _layout->addWidget(rb);
        group_.addButton(rb, i);
        rb->setChecked(i==cell_->val());
        QRadioButton::connect(rb, _SLOT_(QRadioButton,toggled,bool),
                [this,i](bool val)->void { if (val) onChangedValue(i); });
    }
    setLayout(_layout);
    setTitle(_headline);
}

void QcrRadioBox::doSetValue(int val)
{
    ASSERT(val>=0);
    ASSERT(val<size());
    if (val==doGetValue())
        return;
    for (int i=0; i<size(); ++i)
        buttons_[i]->setChecked(i==val);
}

//  ***********************************************************************************************
//! @class QcrLineEdit

QcrLineEdit::QcrLineEdit(const QString& _name, const QString& val)
    : QcrSingleValue<QString> {_name, val}
{
    doSetValue(cell_->val());
    // For unknown reason, hasFocus() is not always false when setText is called programmatically;
    // therefore we must use another criterion to distinuish user actions from other calls.
    // The following works, but has the drawback that a user action is logged not only as such,
    // but also in a second line as if there were an indirect call.
    QLineEdit::connect(this, _SLOT_(QLineEdit,textEdited,const QString&),
            [this](const QString& val)->void {
                onChangedValue(val); });
    QLineEdit::connect(this, _SLOT_(QLineEdit,textChanged,const QString&),
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
    : QcrSingleValue<int> {_name, 0}
{
    doSetValue(cell_->val());
    QTabWidget::connect(this, &QTabWidget::currentChanged, [this](int val) {
            if (spuriousCall_)
                return;
            onChangedValue(val, tabText(val)); });
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
