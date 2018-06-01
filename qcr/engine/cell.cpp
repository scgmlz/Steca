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
