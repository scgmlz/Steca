//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/console.h
//! @brief     Defines class CommandRegistry
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#ifndef COMMAND_REGISTRY_H
#define COMMAND_REGISTRY_H

#include <QString>
#include <QTextStream>
#include <map>

class QcrCommandable;

//! Holds console (= terminal = command-line) commands to be defined and executed by class Console.
class CommandRegistry {
public:
    CommandRegistry() = delete;
    CommandRegistry(const CommandRegistry&) = delete;
    CommandRegistry(const QString& _name) : name_{_name} {}
    QString learn(const QString&, QcrCommandable*);
    void forget(const QString&);
    QcrCommandable* find(const QString& name);
    void dump(QTextStream&) const;
    QString name() const { return name_; }
    int size() const { return widgets_.size(); }
private:
    const QString name_;
    std::map<const QString, QcrCommandable*> widgets_;
    std::map<const QString, int> numberedEntries_;
};

#endif // COMMAND_REGISTRY_H
