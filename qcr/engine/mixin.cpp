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
#include "qcr/base/qcrexception.h"
#include "qcr/engine/console.h"
#include <QAction>
#include <QDebug>

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
    if (const QWidget* w = dynamic_cast<const QWidget*>(&object()))
        if (w->isVisible())
            remake_();
}


//  ***********************************************************************************************
//! @class QcrRoot

QcrRoot::QcrRoot(QObject& object, const QString& name)
    : QcrMixin {object, name}
{
    gRoot = this;
}

void QcrRoot::remakeAll(const QString& whence)
{
    qDebug() << "gRoot->remakeAll < " << whence;
    remake();
    for (QWidget* w: object().findChildren<QWidget*>())
        if (QcrMixin* m = dynamic_cast<QcrMixin*>(w))
            m->remake();
}


//  ***********************************************************************************************
//! @class QcrSettable

QcrSettable::QcrSettable(QObject& object, const QString& name, bool _modal)
    : QcrMixin {object, gConsole->learn(name, this)} // console may change name (expand macros)
{}

// This function looks trivial, but it should not be inlined because it allow us to
// keep console.h out of single_value.h and widgets/controls.h

void QcrSettable::doLog(bool userCalled, const QString& msg)
{
    gConsole->log2(userCalled, msg);
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
