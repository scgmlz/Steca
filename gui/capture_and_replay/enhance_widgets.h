// ************************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/capture_and_replay/enhance_widgets.h
//! @brief     Defines functions that return new Qt objects
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************************************

#ifndef ENHANCE_WIDGETS_H
#define ENHANCE_WIDGETS_H

#include <functional> // no auto rm
#include <QDialog>

//! Mix-in for control widgets that can be changed by a console command.
class CSettable {
public:
    CSettable() = delete;
    CSettable(CSettable&) = delete;
    CSettable(const QString& name);
    ~CSettable();
    virtual void onCommand(const QStringList&) = 0;
    const QString& name() const { return name_; }
private:
    const QString name_;
};

//! Mix-in for modal dialogs.
class CModal { // TODO try private
public:
    CModal() = delete;
    CModal(CModal&) = delete;
    CModal(const QString& name);
    ~CModal();
};

//! A modeless dialog with support for capture&replay.
class CModelessDialog : public QDialog, public CSettable {
public:
    CModelessDialog(QWidget* parent, const QString& name);
    virtual void onCommand(const QStringList&);
private:
    void closeEvent(QCloseEvent*);
};

#endif // ENHANCE_WIDGETS_H
