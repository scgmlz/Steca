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

void QcrMixin::remake()
{
    if (const QWidget* w = dynamic_cast<const QWidget*>(&object())) {
        if (w->isVisible()) {
            remake_();
        } else
            ;
    }
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
    for (QWidget* w: object().findChildren<QWidget*>()) {
        if (QcrMixin* m = dynamic_cast<QcrMixin*>(w))
            m->remake();
    }
    // qDebug() << "/gRoot->remakeAll < " << whence;
}


//  ***********************************************************************************************
//! @class QcrSettable

QcrSettable::QcrSettable(QObject& object, const QString& name, bool _modal)
    : QcrMixin {object, gConsole->learn(name, this)}
    , modal_(_modal)
{}

QcrSettable::~QcrSettable()
{
    if (!modal_)
        gConsole->forget(name());
}

void QcrSettable::doLog(bool softwareCalled, const QString& msg)
{
    gConsole->log2(!softwareCalled, msg);
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
