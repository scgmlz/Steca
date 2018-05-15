//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/widgets/views.cpp
//! @brief     Implements extensions of Qt widgets
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#include "views.h"
#include "qcr/engine/debug.h"

void WidgetCell::recompute()
{
    qwner_->remake();
}

void QcrMixin::remake()
{
    qDebug() << "REMAKE " << cell()->name();
    for (QObject* o: owner_->children())
        if (QcrMixin* m = dynamic_cast<QcrMixin*>(o))
            m->remake();
}
