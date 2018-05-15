//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/widgets/controls.h
//! @brief     Defines functions that return new Qt objects
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#ifndef CONTROLS_H
#define CONTROLS_H

#include <QtGlobal>
#include "qcr/engine/single_value.h"
#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpinBox>
#include <QToolButton>

class QcrAction : public QAction {
public:
    QcrAction(const QString& text);
protected:
    QString tooltip_;
    bool hasFocus();
};

//! Trigger, for use in buttons or menu entries, that can also be activated by console command.
class QcrTrigger : public QcrAction, protected QcrSettable {
public:
    QcrTrigger(const QString& name, const QString& text, const QString& iconFile="");
    QcrTrigger(const QString& name, const QString& text, const QString& iconFile,
               const QKeySequence& shortcut);
    void executeConsoleCommand(const QString&) override;
};

//! Toggle, for use in buttons or menu entries, that can also be switched by console command.
class QcrToggle : public QcrAction, public QcrControl<bool> {
public:
    QcrToggle(const QString& name, const QString& text, bool on,
              const QString& iconFile="", const QKeySequence& shortcut = {});
    QcrToggle(const QString& name, SingleValueCell<bool>* cell, const QString& text,
              const QString& iconFile="", const QKeySequence& shortcut = {});
    bool getValue() const final { return isChecked(); }
private:
    void doSetValue(bool val) final { setChecked(val); }
    // hide some member functions of QAction:
    bool isChecked() const { return QAction::isChecked(); }
    void setChecked(bool val) { QAction::setChecked(val); }
};

//! Button with text display and associated action.
class QcrTextButton : public QToolButton {
public:
    QcrTextButton(QcrAction*);
};

//! Button with icon and associated action.
class QcrIconButton : public QToolButton {
public:
    QcrIconButton(QcrAction*);
};

//! Named integer-valued spin box that can be set by console command.
class QcrSpinBox : public QSpinBox, public QcrControl<int> {
    Q_OBJECT
public:
    QcrSpinBox(const QString& name, int ndigits,
               bool withDot, int min = INT_MIN, int max = INT_MAX, const QString& tooltip="");
    QcrSpinBox(const QString& name, SingleValueCell<int>* cell, int ndigits,
               bool withDot, int min = INT_MIN, int max = INT_MAX, const QString& tooltip="");
    void executeConsoleCommand(const QString&) override;
    int getValue() const final { return value(); }
signals:
    void valueReleased(int); //! Improving over valueChanged, do not signal intermediate states
private:
    void mouseReleaseEvent(QMouseEvent*) override;
    void reportChange();
    int reportedValue_;
    void doSetValue(int val) final { setValue(val); }
    // hide some member functions of QSpinBox:
    int value() const { return QSpinBox::value(); }
    void setValue(int val) { QSpinBox::setValue(val); }
};

//! Named double-valued spin box that can be set by console command.
class QcrDoubleSpinBox : public QDoubleSpinBox, public QcrControl<double> {
    Q_OBJECT
public:
    QcrDoubleSpinBox(const QString& name, int ndigits,
                     double min = LLONG_MIN, double max = LLONG_MAX);
    QcrDoubleSpinBox(const QString& name, SingleValueCell<double>* cell, int ndigits,
                     double min = LLONG_MIN, double max = LLONG_MAX);
    void executeConsoleCommand(const QString&) override;
    double getValue() const final { return value(); }
signals:
    void valueReleased(double); //! Improving over valueChanged, do not signal intermediate states
private:
    void mouseReleaseEvent(QMouseEvent*) override;
    void reportChange();
    double reportedValue_;
    void doSetValue(double val) final { setValue(val); }
    // hide some member functions of QDoubleSpinBox:
    double value() const { return QDoubleSpinBox::value(); }
    void setValue(double val) { QDoubleSpinBox::setValue(val); }
};

//! Named check box that can be set by console command.
class QcrCheckBox : public QCheckBox, public QcrControl<bool> {
public:
    QcrCheckBox(const QString& name, const QString& text, bool val=false);
    QcrCheckBox(const QString& name, const QString& text, SingleValueCell<bool>* cell);
    bool getValue() const final { return isChecked(); }
private:
    void doSetValue(bool val) final { setChecked(val); }
    // hide some member functions of QCheckBox:
    bool isChecked() const { return QCheckBox::isChecked(); }
    void setChecked(bool val) { QCheckBox::setChecked(val); }
};

//! Named radio button that can be set by console command.
class QcrRadioButton : public QRadioButton, public QcrControl<bool> {
public:
    QcrRadioButton(const QString& name, const QString& text);
    bool getValue() const final { return isChecked(); }
private:
    void doSetValue(bool val) final { setChecked(val); }
    // hide some member functions of QRadioButton:
    bool isChecked() const { return QRadioButton::isChecked(); }
    void setChecked(bool val) { QRadioButton::setChecked(val); }
};

//! Named non-editable combo box that can be set by console command.
class QcrComboBox : public QComboBox, public QcrControl<int> {
public:
    QcrComboBox(const QString& name, const QStringList& items = {});
    int getValue() const final { return currentIndex(); }
    void addItems(const QStringList& texts);
private:
    void doSetValue(int val) final { setCurrentIndex(val); }
    // hide some member functions of QComboBox:
    int currentIndex() const { return QComboBox::currentIndex(); }
    void setCurrentIndex(int val) { QComboBox::setCurrentIndex(val); }
    void setCurrentText(const QString&) = delete;
    void setEditable() = delete; // stay with default: editable=false
    void addItem(const QIcon&, const QVariant&) = delete; // reimplement if needed, cf. addItems
    void addItem(const QIcon&, const QString&, const QVariant&) = delete;
};

//! Named line edit that can be set by console command.
class QcrLineEdit : public QLineEdit, public QcrControl<QString> {
public:
    QcrLineEdit(const QString& name, const QString& val = "");
    QString getValue() const final { return text(); }
private:
    void doSetValue(QString val) final { setText(val); }
    // hide some member functions of QLineEdit:
    QString text() const { return QLineEdit::text(); }
    void setText(QString val) { QLineEdit::setText(val); }
};

//! Named tab widget that can be set by console command.
class QcrTabWidget : public QTabWidget, public QcrControl<int> {
public:
    QcrTabWidget(const QString& name);
    int getValue() const final { return currentIndex(); }
    void addTab(QWidget* page, const QString& label);
private:
    SingleValueCell<int> defaultCell;
    void doSetValue(int val) final { setCurrentIndex(val); }
    // hide some member functions of QTabWidget:
    int currentIndex() const { return QTabWidget::currentIndex(); }
    void setCurrentIndex(int val);
    void setCurrentWidget(QWidget*) = delete;
};

//! Dialog, for modal use.
class QcrDialog : public QDialog, protected QcrModal {
public:
    QcrDialog(QWidget* parent, const QString& caption);
    ~QcrDialog();
    int exec() override;
    void executeConsoleCommand(const QString&) override;
};

//! File dialog, for modal use, with console commands to select files and to close the dialog.
class QcrFileDialog : public QFileDialog, protected QcrModal {
public:
    QcrFileDialog(
        QWidget* parent, const QString& caption, const QString& directory, const QString& filter);
    ~QcrFileDialog();
    int exec() override;
    void executeConsoleCommand(const QString&) override;
};

#endif // CONTROLS_H
