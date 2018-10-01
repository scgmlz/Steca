//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/widgets/controls.h
//! @brief     Defines enhanced control widgets like QcrAction, QcrSpinBox, and many others
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#ifndef CONTROLS_H
#define CONTROLS_H

#include "qcr/engine/single_value.h"
#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpinBox>
#include <QToolButton>
#include <QtGlobal>

class QcrAction : public QAction {
public:
    QcrAction(const QString& text);
protected:
    QString tooltip_;
    bool hasFocus();
};

//! Trigger, for use in buttons or menu entries, that can also be activated by console command.
class QcrTrigger : public QcrAction, public QcrSettable {
public:
    QcrTrigger(const QString& name, const QString& text, const QString& iconFile="");
    QcrTrigger(const QString& name, const QString& text, const QString& iconFile,
               const QKeySequence& shortcut);
    void executeConsoleCommand(const QString&) override;
    void setTriggerHook(std::function<void()> triggerHook) { triggerHook_ = triggerHook; }
private:
    std::function<void()> triggerHook_ = [](){;};
};

//! Trigger button with text display and associated action.
class QcrTextTriggerButton : public QToolButton, public QcrMixin {
public:
    QcrTextTriggerButton(QcrTrigger*);
};

//! Trigger button with icon and associated action.
class QcrIconTriggerButton : public QToolButton, public QcrMixin {
public:
    QcrIconTriggerButton(QcrTrigger*);
};

//! Toggle, for use in buttons or menu entries, that can also be switched by console command.
class QcrToggle : public QcrAction, public QcrControl<bool> {
public:
    QcrToggle(const QString& name, const QString& text, bool on,
              const QString& iconFile="", const QKeySequence& shortcut = {});
    QcrToggle(const QString& name, QcrCell<bool>* cell, const QString& text,
              const QString& iconFile="", const QKeySequence& shortcut = {});
    bool doGetValue() const final { return isChecked(); }
private:
    void initToggle(const QString& iconFile, const QKeySequence& shortcut);
    void doSetValue(bool val) final { setChecked(val); }
    // hide some member functions of QAction:
    void setChecked(bool val) { QAction::setChecked(val); }
};

//! Toggle button with text display and associated action.
class QcrTextToggleButton : public QToolButton, public QcrMixin {
public:
    QcrTextToggleButton(QcrToggle*);
};

//! Toggle button with icon and associated action.
class QcrIconToggleButton : public QToolButton, public QcrMixin {
public:
    QcrIconToggleButton(QcrToggle*);
};

//! Named integer-valued spin box that can be set by console command.
class QcrSpinBox : public QSpinBox, public QcrControl<int> {
public:
    QcrSpinBox(const QString& name, QcrCell<int>* cell, int ndigits, bool withDot = false,
               int min = INT_MIN, int max = INT_MAX, const QString& tooltip="");
    void executeConsoleCommand(const QString&) override;
    int doGetValue() const final { return value(); }
private:
    void initSpinBox(int ndigits, bool withDot, int min, int max, const QString& tooltip);
    void mouseReleaseEvent(QMouseEvent*) override;
    void doSetValue(int val) final { setValue(val); }
    // hide some member functions of QSpinBox:
    void setValue(int val) { QSpinBox::setValue(val); }
};

//! Named double-valued spin box that can be set by console command.
class QcrDoubleSpinBox : public QDoubleSpinBox, public QcrControl<double> {
public:
    QcrDoubleSpinBox(const QString& name, QcrCell<double>* cell, int nDigits, int nDecimals,
                     double min = LLONG_MIN, double max = LLONG_MAX, const QString& tooltip="");
    void executeConsoleCommand(const QString&) override;
    double doGetValue() const final { return value(); }
private:
    void initDoubleSpinBox(
        int nDigits, int nDecimals, double min, double max, const QString& tooltip);
    void mouseReleaseEvent(QMouseEvent*) override;
    void doSetValue(double val) final { setValue(val); }
    // hide some member functions of QDoubleSpinBox:
    void setValue(double val) { QDoubleSpinBox::setValue(val); }
};

//! Named check box that can be set by console command.
class QcrCheckBox : public QCheckBox, public QcrControl<bool> {
public:
    QcrCheckBox(const QString& name, const QString& text, QcrCell<bool>* cell);
    bool doGetValue() const final { return isChecked(); }
private:
    void doSetValue(bool val) final { setChecked(val); }
    // hide some member functions of QCheckBox:
    void setChecked(bool val) { QCheckBox::setChecked(val); }
};

//! Named radio button that can be set by console command.
class QcrRadioButton : public QRadioButton, public QcrControl<bool> {
public:
    QcrRadioButton(const QString& name, const QString& text, bool val=false);
    QcrRadioButton(const QString& name, const QString& text, QcrCell<bool>* cell);
    bool doGetValue() const final { return isChecked(); }
private:
    void doSetValue(bool val) final { setChecked(val); }
    // hide some member functions of QRadioButton:
    void setChecked(bool val) { QRadioButton::setChecked(val); }
};

//! Named non-editable combo box that can be set by console command.
class QcrComboBox : public QComboBox, public QcrControl<int> {
public:
    QcrComboBox(const QString& name, QcrCell<int>* cell, std::function<QStringList()> makeTags);
    // TODO add simplified API with fixed tag list
    int doGetValue() const final { return (int)currentIndex(); }
    void remake() override;
private:
    QStringList tags_;
    std::function<QStringList()> makeTags_;
    bool spuriousCall_ {false};
    void doSetValue(int val) final { setCurrentIndex((int)val); }
    // hide some member functions of QComboBox:
    void setCurrentIndex(int val) { QComboBox::setCurrentIndex(val); }
    void setCurrentText(const QString&) = delete;
    void setEditable() = delete; // stay with default: editable=false
    void addItems(const QStringList&) = delete;
    void addItem(const QIcon&, const QVariant&) = delete;
    void addItem(const QIcon&, const QString&, const QVariant&) = delete;
};

//! Named line edit that can be set by console command.
class QcrLineEdit : public QLineEdit, public QcrControl<QString> {
public:
    QcrLineEdit(const QString& name, const QString& val = "");
    QString doGetValue() const final { return text(); }
private:
    void doSetValue(QString val) final;
    // hide some member functions of QLineEdit:
    void setText(QString val) { QLineEdit::setText(val); }
};

//! Named tab widget that can be set by console command.
class QcrTabWidget : public QTabWidget, public QcrControl<int> {
public:
    QcrTabWidget(const QString& name);
    int doGetValue() const final { return currentIndex(); }
    void addTab(QWidget* page, const QString& label);
    void setTabEnabled(int, bool);
    bool anyEnabled() const;
private:
    bool spuriousCall_ {false};
    void doSetValue(int val) final { setCurrentIndex(val); }
    // hide some member functions of QTabWidget:
    void setCurrentIndex(int val);
    void setCurrentWidget(QWidget*) = delete;
};

#endif // CONTROLS_H
