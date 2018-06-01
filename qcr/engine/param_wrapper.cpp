//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/param_wrapper.cpp
//! @brief     Implements global function used by class ParamWrapper
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#include "param_wrapper.h"
#include "qcr/engine/mixin.h"
#include <QtDebug>

// TODO RECONSIDER: move out of global namespace
void remakeAll()
{
    ASSERT(gRoot);
    gRoot->fullRemake();
}
