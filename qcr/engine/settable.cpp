//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/settable.cpp
//! @brief     Implements classes QcrSettable, QcrModal, QcrModelessDialog
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#include "settable.h"
#include "qcr/engine/console.h"
#include "qcr/engine/qcrexception.h"
#include <QDebug>

//  ***********************************************************************************************
//! @class QcrMixin

QcrMixin::QcrMixin(QObject& object, const QString& name)
    : object_ {object}
{
    object_.setObjectName(name);
}

QcrMixin::~QcrMixin()
{
    gConsole->forget(name());
}

void QcrMixin::recursiveRemake()
{
    postProcess();
    static int indent = 0;
    qDebug() << QString(indent, ' ') + "REMAKE " << name() << " (" << object().children().size() << " children)";
    ++indent;
    for (QObject* o: object().children()) {
        if (QcrMixin* m = dynamic_cast<QcrMixin*>(o))
            m->recursiveRemake();
        else
            qDebug() << QString(indent, ' ') + "cannot remake " << o->objectName();
    }
    -- indent;
    preProcess();
}

//  ***********************************************************************************************
//! @class QcrSettable

QcrSettable::QcrSettable(QObject& object, const QString& name)
    : QcrMixin {object, gConsole->learn(name, this)}
{}

void QcrSettable::doLog(bool softwareCalled, const QString& msg)
{
    gConsole->log2(!softwareCalled, msg);
}


//  ***********************************************************************************************
//! @class QcrModal

QcrModal::QcrModal(const QString& name)
{
    gConsole->call("@push "+name);
}

QcrModal::~QcrModal()
{
    gConsole->log("@close");
    gConsole->call("@pop");
}


//  ***********************************************************************************************
//! @class QcrModelessDialog

QcrModelessDialog::QcrModelessDialog(QWidget* parent, const QString& name)
    : QDialog {parent}
    , QcrSettable {*this, name}
{
    setModal(false);
}

void QcrModelessDialog::closeEvent(QCloseEvent* event)
{
    deleteLater();
}

void QcrModelessDialog::executeConsoleCommand(const QString& arg)
{
    if (arg!="close")
        throw QcrException("Unexpected command in ModelessDialog "+name());
    close();
}
