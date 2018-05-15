//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/mixin.cpp
//! @brief     Implements classes QcrMixin, QcrRoot, QcrSettable, QcrModal, QcrModelessDialog
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#include "mixin.h"
#include "qcr/engine/console.h"
#include "qcr/base/qcrexception.h"
#include <QDebug>

QcrRoot* gRoot {nullptr};

//  ***********************************************************************************************
//! @class QcrMixin

QcrMixin::QcrMixin(QObject& object, const QString& name)
    : object_ {object}
{
    object_.setObjectName(name);
}

void QcrMixin::remake()
{
    const QWidget* w = dynamic_cast<const QWidget*>(&object());
    bool vis = w && w->isVisible();
    qDebug() << "remake " << name() << " (visible: " << vis << ")";
}

//  ***********************************************************************************************
//! @class QcrRoot

QcrRoot::QcrRoot(QObject& object, const QString& name)
    : QcrMixin {object, name}
{
    gRoot = this;
}

void QcrRoot::fullRemake()
{
    for (QWidget* w: object().findChildren<QWidget*>()) {
        if (QcrMixin* m = dynamic_cast<QcrMixin*>(w))
            m->remake();
    }
}
//  ***********************************************************************************************
//! @class QcrSettable

QcrSettable::QcrSettable(QObject& object, const QString& name)
    : QcrMixin {object, gConsole->learn(name, this)}
{}

QcrSettable::~QcrSettable()
{
    gConsole->forget(name());
}

void QcrSettable::doLog(bool softwareCalled, const QString& msg)
{
    gConsole->log2(!softwareCalled, msg);
}


//  ***********************************************************************************************
//! @class QcrModal

QcrModal::QcrModal(QObject& object, const QString& name)
    : QcrSettable {object, name}
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
