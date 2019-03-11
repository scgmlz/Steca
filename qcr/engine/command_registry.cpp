//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/console.cpp
//! @brief     Implements class CommandRegistry
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#include "qcr/engine/command_registry.h"
#include "qcr/base/debug.h"

QString CommandRegistry::learn(const QString& name, QcrCommandable* widget)
{
    ASSERT(name!=""); // empty name only allowed for non-settable QcrBase
    // qDebug() << "Registry " << name_ << " learns '" << name;
    QString ret = name;
    if (ret.contains("#")) {
        auto numberedEntry = numberedEntries_.find(name);
        int idxEntry = 1;
        if (numberedEntry==numberedEntries_.end())
            numberedEntries_[name] = idxEntry;
        else
            idxEntry = ++(numberedEntry->second);
        ret.replace("#", QString::number(idxEntry));
    }
    if (widgets_.find(ret)!=widgets_.end())
        qFatal("Duplicate widget registry entry '%s'", CSTRI(ret));
    widgets_[ret] = widget;
    return ret;
}

void CommandRegistry::forget(const QString& name)
{
    // qDebug() << "Registry " << name_ << "(" << widgets_.size() << ") forgets '"  << name;
    auto it = widgets_.find(name);
    if (it==widgets_.end())
        qFatal("Cannot deregister, there is no entry '%s' in the widget registry '%s'",
               CSTRI(name), CSTRI(name_));
    widgets_.erase(it);
}

QcrCommandable* CommandRegistry::find(const QString& name)
{
    auto entry = widgets_.find(name);
    if (entry==widgets_.end())
        return {};
    return entry->second;
}

void CommandRegistry::dump(QTextStream& stream) const
{
    for (auto it: widgets_)
        stream << " " << it.first;
    stream << "\n";
}
