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
#include <QtDebug>

CellSignaller* gRoot;

Cell::stamp_t ValueCell::latestTimestamp__ = 0;

void Cell::add_client(Cell* src) {
    if (clients_.find(src)!=clients_.end())
        qFatal("duplicate cell client entry");
    clients_.insert(src);
}

void Cell::rm_client(Cell* src) {
    auto it = clients_.find(src);
    if (it==clients_.end())
        qFatal("failed removing cell client entry");
    clients_.erase(it);
}

Cell::stamp_t Cell::update()
{
    stamp_t t_new = timestamp_;
    for (Cell* cli: clients_)
        t_new = std::max(t_new, cli->update());
    if (t_new>timestamp_) {
        recompute();
        timestamp_ = t_new;
    }
    return timestamp_;
}

void Cell::connectAction(std::function<void()>&& f)
{
    actionsOnChange_.push_back(std::move(f));
}

void Cell::actOnChange()
{
    for (const std::function<void()>& f: actionsOnChange_)
        f();
}
