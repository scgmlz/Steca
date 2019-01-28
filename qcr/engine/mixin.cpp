//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/mixin.cpp
//! @brief     Implements classes QcrBaseMixin, QcrRootMixin, QcrRegisteredMixin, QcrModalMixin, QcrModelessDialog
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

QcrRootMixin* gRoot {nullptr};

//  ***********************************************************************************************
//! @class QcrBaseMixin

QcrBaseMixin::QcrBaseMixin(QObject* object, const QString& name)
    : object_ {object}
{
    object_->setObjectName(name);
}

//! Calls the hook remake_, provided the associated QObject is a visible QWidget, or a QAction.

//! Use is mostly from QcrRootMixin::remakeAll().
//!
//! Children may provide additional remake functionality; they override this, and include
//! a call to QcrBaseMixin::remake(). This is currently done in QcrComboBox::remake().
void QcrBaseMixin::remake()
{
    const QWidget* w = dynamic_cast<const QWidget*>(object());
    if ((w && w->isVisible()) || dynamic_cast<const QAction*>(object()))
        remake_();
}


//  ***********************************************************************************************
//! @class QcrRootMixin

QcrRootMixin::QcrRootMixin(QObject* object)
    : QcrBaseMixin {object, "root"}
{
    gRoot = this;
}

static int remakeLoops {0};

void QcrRootMixin::remakeAll()
{
    ++remakeLoops;
    if (remakeLoops>1)
        qFatal("BUG: circular remakeAll");
    remake();
    for (QWidget* w: object()->findChildren<QWidget*>())
        if (QcrBaseMixin* m = dynamic_cast<QcrBaseMixin*>(w))
            m->remake();
    --remakeLoops;
}


//  ***********************************************************************************************
//! @class QcrRegisteredMixin

QcrRegisteredMixin::QcrRegisteredMixin(QObject* object, const QString& name, bool _modal)
    : QcrBaseMixin {object, gConsole->learn(name, this)} // console may change name (expand macros)
{}

void QcrRegisteredMixin::doLog(const QString& msg)
// Trivial, but not inlined to isolate dependence on console.h.
{
    gConsole->log(msg);
}

//! Returns true if the value of *this is not to be stored as part of the QSettings.
bool QcrRegisteredMixin::adhoc() const
{
    return name().left(6)=="adhoc_";
}


//  ***********************************************************************************************
//! @class QcrModelessDialog (= persistent spawned popup)

QcrModelessDialog::QcrModelessDialog(QWidget* parent, const QString& name)
    : QDialog {parent}
    , QcrRegisteredMixin {this, name}
{
    setModal(false);
}

void QcrModelessDialog::closeEvent(QCloseEvent* event)
{
    deleteLater();
}

void QcrModelessDialog::setFromCommand(const QString& arg)
{
    if (arg!="close")
        throw QcrException("Unexpected command in ModelessDialog "+name());
    close();
}
