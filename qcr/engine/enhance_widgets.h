//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/enhance_widgets.h
//! @brief     Defines classes CSettable, CModal, CModelessDialog
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#ifndef ENHANCE_WIDGETS_H
#define ENHANCE_WIDGETS_H

#include "qcr/engine/string_ops.h"
#include <functional> // no auto rm
#include <QDialog>

//! Mix-in for control widgets that can be changed by a console command.
class CSettable {
public:
    virtual void onCommand(const QString&) = 0;
    const QString& name() const { return name_; }
protected:
    CSettable() = delete;
    CSettable(const CSettable&) = delete;
    CSettable(const QString& name);
    ~CSettable();
    void doLog(bool softwareCalled, const QString& msg);
private:
    const QString name_;
};

//! Base class for all Qcr widgets that hold a single value.
template<class T>
class QcrControl : protected CSettable {
public:
    QcrControl(const QString& name)
        : CSettable(name) {}
    void programaticallySetValue(T val) {
        softwareCalling_ = true;
        doSetValue(val);
        softwareCalling_ = false;
    }
    virtual T getValue() const = 0;
protected:
    void init() {
        reportedValue_ = getValue();
        doLog(true, "initialization: "+name()+" "+strOp::to_s(reportedValue_));
    }
    void onChangedValue(T val) {
        if (val!=reportedValue_) {
            doLog(softwareCalling_, name()+" "+strOp::to_s(val));
            reportedValue_ = val;
        }
    }
private:
    virtual void doSetValue(T) = 0;
    bool softwareCalling_ = false;
    T reportedValue_;
};

//! Mix-in for modal dialogs.
class CModal {
protected:
    CModal() = delete;
    CModal(const CModal&) = delete;
    CModal(const QString& name);
    ~CModal();
};

//! A modeless dialog with support for capture&replay.
class CModelessDialog : protected QDialog, protected CSettable {
public:
    virtual void onCommand(const QString&);
protected:
    CModelessDialog(QWidget* parent, const QString& name);
private:
    void closeEvent(QCloseEvent*);
};

#endif // ENHANCE_WIDGETS_H
