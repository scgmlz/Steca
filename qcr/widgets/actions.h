//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/widgets/actions.h
//! @brief     Defines enhanced action widgets like Qcr(Icon|Text)(Trigger|Toggle)Button
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#ifndef ACTIONS_H
#define ACTIONS_H

#include "qcr/engine/single_value.h"
#include <QAction>
#include <QToolButton>
#include <QtGlobal>

class QcrAction : public QAction {
public:
    QcrAction(const QString& text);
protected:
    QString tooltip_;
};

//! Trigger, for use in buttons or menu entries, that can also be activated by console command.
class QcrTrigger : public QcrRegistered, public QcrAction {
public:
    QcrTrigger(const QString& name, const QString& text, const QString& iconFile="");
    QcrTrigger(const QString& name, const QString& text, const QString& iconFile,
               const QKeySequence& shortcut);
    void setFromCommand(const QString&) override;
    void setTriggerHook(std::function<void()> triggerHook) { triggerHook_ = triggerHook; }
private:
    std::function<void()> triggerHook_ = [](){;};
};

//! Trigger button with text display and associated action.
class QcrTextTriggerButton : public QcrBase, public QToolButton {
public:
    QcrTextTriggerButton(QcrTrigger*);
    QcrTextTriggerButton(const QString& name, const QString& text);
    ~QcrTextTriggerButton();
    QcrTrigger* trigger() const { return trigger_; }
private:
    QcrTextTriggerButton(QcrTrigger*, bool ownsTrigger);
    QcrTrigger*const trigger_;
    const bool ownsTrigger_;
};

//! Trigger button with icon and associated action.
class QcrIconTriggerButton : public QcrBase, public QToolButton {
public:
    QcrIconTriggerButton(QcrTrigger*);
    QcrIconTriggerButton(const QString& name, const QString& text, const QString& iconFile);
    ~QcrIconTriggerButton();
    QcrTrigger* trigger() const { return trigger_; }
private:
    QcrIconTriggerButton(QcrTrigger*, bool ownsTrigger);
    QcrTrigger*const trigger_;
    const bool ownsTrigger_;
};

//! Toggle, for use in buttons or menu entries, that can also be switched by console command.
class QcrToggle : public QcrSingleValue<bool>, public QcrAction {
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
class QcrTextToggleButton : public QcrBase, public QToolButton {
public:
    QcrTextToggleButton(QcrToggle*);
    QcrTextToggleButton(const QString& name, const QString& text, bool on);
    ~QcrTextToggleButton();
    QcrToggle* toggle() const { return toggle_; }
private:
    QcrTextToggleButton(QcrToggle*, bool ownsToggle);
    QcrToggle*const toggle_;
    const bool ownsToggle_;
};

//! Toggle button with icon and associated action.
class QcrIconToggleButton : public QcrBase, public QToolButton {
public:
    QcrIconToggleButton(QcrToggle*);
    QcrIconToggleButton(const QString& name, const QString& text, bool on,
                        const QString& iconFile, const QKeySequence& shortcut = {});
    QcrIconToggleButton(const QString& name, QcrCell<bool>* cell, const QString& text,
                        const QString& iconFile="", const QKeySequence& shortcut = {});
    ~QcrIconToggleButton();
private:
    QcrIconToggleButton(QcrToggle*, bool ownsToggle);
    QcrToggle*const toggle_;
    const bool ownsToggle_;
};

#endif // ACTIONS_H
