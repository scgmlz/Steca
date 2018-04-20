//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      qcr/engine/enhance_widgets.h
//! @brief     Defines functions that return new Qt objects
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef ENHANCE_WIDGETS_H
#define ENHANCE_WIDGETS_H

#include <functional> // no auto rm
#include <QDialog>

//! Mix-in for control widgets that can be changed by a console command.
class CSettable {
public:
    virtual void onCommand(const QStringList&) = 0;
    const QString& name() const { return name_; }
protected:
    CSettable() = delete;
    CSettable(const CSettable&) = delete;
    CSettable(const QString& name);
    ~CSettable();
private:
    const QString name_;
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
    virtual void onCommand(const QStringList&);
protected:
    CModelessDialog(QWidget* parent, const QString& name);
private:
    void closeEvent(QCloseEvent*);
};

#endif // ENHANCE_WIDGETS_H
