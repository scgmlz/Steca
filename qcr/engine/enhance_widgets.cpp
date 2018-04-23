//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      qcr/engine/enhance_widgets.cpp
//! @brief     Implements functions that return new Qt objects
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "qcr/engine/console.h"
#include "qcr/engine/qcrexception.h"
#include <QDebug>

//  ***********************************************************************************************
//! @class CSettable

CSettable::CSettable(const QString& name)
    : name_ {gConsole->learn(name, this)}
{
}

CSettable::~CSettable()
{
    gConsole->forget(name_);
}

//  ***********************************************************************************************
//! @class CModal

CModal::CModal(const QString& name)
{
    gConsole->call("@push "+name);
}

CModal::~CModal()
{
    gConsole->log("@close");
    gConsole->call("@pop");
}

//  ***********************************************************************************************
//! @class CModelessDialog

CModelessDialog::CModelessDialog(QWidget* parent, const QString& name)
    : QDialog(parent)
    , CSettable(name)
{
    setModal(false);
}

void CModelessDialog::closeEvent(QCloseEvent* event)
{
    deleteLater();
}

void CModelessDialog::onCommand(const QStringList& args)
{
    if (args[0]!="close")
        throw QCRException("Unexpected command in ModelessDialog "+name());
    close();
}
