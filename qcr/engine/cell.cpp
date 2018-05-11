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

Cell::stamp_t FinalCell::latestTimestamp__ = 0;

void Cell::add_source(Cell* src) {
    if (sources_.find(src)!=sources_.end())
        qFatal("duplicate cell source entry");
    sources_.insert(src);
}

void Cell::rm_source(Cell* src) {
    auto it = sources_.find(src);
    if (it==sources_.end())
        qFatal("failed removing cell source entry");
    sources_.erase(it);
}

Cell::stamp_t Cell::update()
{
    stamp_t t_new = timestamp_;
    for (Cell* src: sources_)
        t_new = std::max(t_new, src->update());
    if (t_new>timestamp_) {
        recompute();
        timestamp_ = t_new;
    }
    return timestamp_;
}

void Cell::connectAction(const std::function<void()>*)
{
}

void Cell::actOnChange()
{
}
