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

#include "qcr/engine/mixin.h"
#include "qcr/base/qcrexception.h"
#include "qcr/engine/console.h"
#include <QAction>

QcrRoot* gRoot {nullptr};

//  ***********************************************************************************************
//! @class QcrMixin

QcrMixin::QcrMixin(QObject& object, const QString& name)
    : object_ {object}
{
    object_.setObjectName(name);
}

QcrMixin::QcrMixin(QObject* object, const QString& name)
    : object_ {*object}
{
    object_.setObjectName(name);
}

void QcrMixin::remake()
{
    const QWidget* w = dynamic_cast<const QWidget*>(&object());
    if ((w && w->isVisible()) || dynamic_cast<const QAction*>(&object()))
        remake_();
}


//  ***********************************************************************************************
//! @class QcrRoot

QcrRoot::QcrRoot(QObject* object)
    : QcrMixin {object}
{
    gRoot = this;
}

static int remakeLoops {0};

void QcrRoot::remakeAll()
{
    ++remakeLoops;
    if (remakeLoops>1)
        qFatal("circular remakeAll, it seems");
    remake();
    for (QWidget* w: object().findChildren<QWidget*>())
        if (QcrMixin* m = dynamic_cast<QcrMixin*>(w))
            m->remake();
    --remakeLoops;
}


//  ***********************************************************************************************
//! @class QcrSettable

QcrSettable::QcrSettable(QObject& object, const QString& name, bool _modal)
    : QcrMixin {object, gConsole->learn(name, this)} // console may change name (expand macros)
{}

// Not inlined to isolate dependence on console.h.

void QcrSettable::doLog(const QString& msg)
{
    gConsole->log(msg);
}


//  ***********************************************************************************************
//! @class QcrModelessDialog (= persistent spawned popup)

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
