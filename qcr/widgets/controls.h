//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/widgets/controls.h
//! @brief     Defines enhanced control widgets like QcrSpinBox, QcrRadioButton, and many others
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
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpinBox>
#include <QtGlobal>

//! Named integer-valued spin box that can be set by console command.
class QcrSpinBox : public QcrSingleValue<int>, public QSpinBox {
public:
    QcrSpinBox(const QString& name, QcrCell<int>* cell, int ndigits, bool withDot = false,
               int min = INT_MIN, int max = INT_MAX, const QString& tooltip="");
    void setFromCommand(const QString&) override;
    int doGetValue() const final { return value(); }
private:
    void initSpinBox(int ndigits, bool withDot, int min, int max, const QString& tooltip);
    void mouseReleaseEvent(QMouseEvent*) override;
    void doSetValue(int val) final { setValue(val); }
    // hide some member functions of QSpinBox:
    void setValue(int val) { QSpinBox::setValue(val); }
};

//! Named double-valued spin box that can be set by console command.
class QcrDoubleSpinBox : public QcrSingleValue<double>, public QDoubleSpinBox {
public:
    QcrDoubleSpinBox(const QString& name, QcrCell<double>* cell, int nDigits, int nDecimals,
                     double min = LLONG_MIN, double max = LLONG_MAX, const QString& tooltip="");
    void setFromCommand(const QString&) override;
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
class QcrCheckBox : public QcrSingleValue<bool>, public QCheckBox {
public:
    QcrCheckBox(const QString& name, const QString& text, QcrCell<bool>* cell);
    bool doGetValue() const final { return isChecked(); }
private:
    void doSetValue(bool val) final { setChecked(val); }
    // hide some member functions of QCheckBox:
    void setChecked(bool val) { QCheckBox::setChecked(val); }
};

//! Named radio button that can be set by console command.
class QcrRadioButton : public QcrSingleValue<bool>, public QRadioButton {
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
class QcrComboBox : public QcrSingleValue<int>, public QComboBox {
public:
    //! Constructor for fixed tag list case.
    QcrComboBox(const QString& name, QcrCell<int>* cell, const QStringList& tags);
    //! Constructor for variable tag list case.
    QcrComboBox(const QString& name, QcrCell<int>* cell,
                const std::function<QStringList()> makeTags);
    int doGetValue() const final { return (int)currentIndex(); }
    void remake() override;
private:
    //! Generic low-level constructor
    QcrComboBox(const QString& name, QcrCell<int>* cell, const bool haveRemakeTagsFunction,
                const QStringList& tags, const std::function<QStringList()> makeTags);
    const bool haveRemakeTagsFunction_;
    QStringList tags_;
    const std::function<QStringList()> makeTags_;
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

//! Group of radio buttons, of which exactly one is activated.
class QcrRadioBox : public QcrSingleValue<int>, public QGroupBox {
public:
    //! Constructor; takes ownership of layout, which is typically provided as "new QVLayout".
    QcrRadioBox(const QString& name, const QString& headline, QcrCell<int>* cell,
                const QStringList& tags, QLayout* layout);
    int doGetValue() const final { return group_.checkedId(); }
    // QRadioButton* buttonAt(int i) { return buttons_.at(i); }
private:
    QStringList tags_;
    QButtonGroup group_;
    std::vector<QRadioButton*> buttons_;
    int size() const { return tags_.size(); }
    void doSetValue(int val) final;
};

//! Named line edit that can be set by console command.
class QcrLineEdit : public QcrSingleValue<QString>, public QLineEdit {
public:
    QcrLineEdit(const QString& name, const QString& val = "");
    QString doGetValue() const final { return text(); }
private:
    void doSetValue(QString val) final;
    // hide some member functions of QLineEdit:
    void setText(QString val) { QLineEdit::setText(val); }
};

//! Named tab widget that can be set by console command.
class QcrTabWidget : public QcrSingleValue<int>, public QTabWidget {
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
