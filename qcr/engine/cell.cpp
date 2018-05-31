//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/cell.cpp
//! @brief     Implements class Cell
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#include "cell.h"
#include "qcr/engine/mixin.h"
#include <QtDebug>

// TODO RECONSIDER: move out of global namespace
void remakeAll()
{
    ASSERT(gRoot);
    gRoot->fullRemake();
}


Cell::stamp_t ValueCell::latestTimestamp__ = 0;


Cell::stamp_t Cell::update()
{
    //stamp_t t_new = timestamp_;
    qDebug() << "Updating " << name() << " t=" << timestamp_;
    recompute();
    /*
    for (Cell* src: sources_)
        t_new = std::max(t_new, src->update());
    if (t_new>timestamp_) {
        recompute();
        timestamp_ = t_new;
    }
    */
    qDebug() << " Updated " << name() << " t=" << timestamp_;
    return timestamp_;
}

//! Appends given function to actionsOnChange_.
void Cell::connectAction(std::function<void()>&& f)
{
    actionsOnChange_.push_back(std::move(f));
}

void Cell::actOnChange()
{
    for (const std::function<void()>& f: actionsOnChange_)
        f();
}
