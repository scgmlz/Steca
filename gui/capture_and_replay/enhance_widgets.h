// ************************************************************************** //
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
// ************************************************************************** //

#ifndef ENHANCE_WIDGETS_H
#define ENHANCE_WIDGETS_H

#include <functional> // no auto rm
#include <QDialog>
#include <QString>

//! Mix-in for classes with named instances
class INamed {
public:
    INamed() = delete;
    INamed(const QString& name) : name_(name) {}
    QString name() const { return name_; }
private:
    const QString name_;
};

//! Mix-in for control widgets that can be changed by a console command.
class CSettable : public INamed {
public:
    CSettable(const QString& name);
    ~CSettable();
    virtual void cmd(const QString&) = 0;
};

//! Mix-in for modal dialogs.
class CModal : public CSettable { // TODO try private
public:
    CModal(const QString& name);
    ~CModal();
};

#endif // ENHANCE_WIDGETS_H
