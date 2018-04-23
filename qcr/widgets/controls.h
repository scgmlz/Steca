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
#include "qcr/engine/enhance_widgets.h"
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

class QcrAction : public QAction, protected CSettable {
public:
    QcrAction(const QString& rawname, const QString& text);
protected:
    QString tooltip_;
};

//! Trigger, for use in buttons or menu entries, that can also be activated by console command.
class QcrTrigger : public QcrAction {
public:
    QcrTrigger(const QString& name, const QString& text, const QString& iconFile="");
    QcrTrigger(const QString& name, const QString& text, const QString& iconFile,
               const QKeySequence& shortcut);
    void onCommand(const QStringList&) override;
};

//! Toggle, for use in buttons or menu entries, that can also be switched by console command.
class QcrToggle : public QcrAction {
public:
    QcrToggle(const QString& name, const QString& text, bool on, const QString& iconFile="");
    QcrToggle(const QString& name, const QString& text, bool on, const QString& iconFile,
              const QKeySequence& shortcut);
    void onCommand(const QStringList&) override;
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
class QcrSpinBox : public QSpinBox, private CSettable {
    Q_OBJECT
public:
    QcrSpinBox(const QString& name, int ndigits, bool withDot, int min = INT_MIN, int max = INT_MAX,
               const QString& tooltip="");
    void onCommand(const QStringList&) override;
signals:
    void valueReleased(int); //! Improving over valueChanged, do not signal intermediate states
private:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void reportChange();
    int reportedValue_;
};

//! Named double-valued spin box that can be set by console command.
class QcrDoubleSpinBox : public QDoubleSpinBox, private CSettable {
    Q_OBJECT
public:
    QcrDoubleSpinBox(const QString& name, int ndigits, double min = LLONG_MIN, double max = LLONG_MAX);
    void onCommand(const QStringList&) override;
signals:
    void valueReleased(double); //! Improving over valueChanged, do not signal intermediate states
private:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void reportChange();
    double reportedValue_;
};

//! Named check box that can be set by console command.
class QcrCheckBox : public QCheckBox, private CSettable {
public:
    QcrCheckBox(const QString& name, const QString& text);
    void onCommand(const QStringList&) override;
};

//! Named radio button that can be set by console command.
class QcrRadioButton : public QRadioButton, private CSettable {
public:
    QcrRadioButton(const QString& name, const QString& text);
    void onCommand(const QStringList&) override;
};

//! Named combo box that can be set by console command.
class QcrComboBox : public QComboBox, private CSettable {
public:
    QcrComboBox(const QString& name, const QStringList& items = {});
    void onCommand(const QStringList&) override;
};

//! Named line edit that can be set by console command (but use XLineEdit for pure display).
class QcrLineEdit : public QLineEdit, private CSettable {
public:
    QcrLineEdit(const QString& name, const QString& val = "");
    void onCommand(const QStringList&) override;
};

//! Named tab widget that can be set by console command.
class QcrTabWidget : public QTabWidget, private CSettable {
public:
    QcrTabWidget(const QString& name);
    void onCommand(const QStringList&) override;
};

//! File dialog, for modal use, with console commands to select files and to close the dialog.
class QcrFileDialog : public QFileDialog, private CModal, CSettable {
public:
    QcrFileDialog(QWidget *parent = Q_NULLPTR, const QString &caption = QString(),
                  const QString &directory = QString(), const QString &filter = QString());
    ~QcrFileDialog();
    int exec() override;
    void onCommand(const QStringList&) override;
};

#endif // CONTROLS_H
