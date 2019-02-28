//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/mixin.cpp
//! @brief     Implements classes QcrBase, QcrRoot, QcrRegistered, QcrModal, QcrModelessDialog
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#include "qcr/engine/mixin.h"
#include "qcr/base/debug.h"
#include "qcr/base/qcrexception.h"
#include "qcr/engine/console.h"
#include <QAction>

QcrRoot* gRoot {nullptr};

//  ***********************************************************************************************
//! @class QcrBase

QcrBase::QcrBase(const QString& name)
{
    setObjectName(name);
}

//! Calls the hook remake_, provided the associated QObject is a visible QWidget, or a QAction.

//! Use is mostly from QcrRoot::remakeAll().
//!
//! Children may provide additional remake functionality; they override this, and include
//! a call to QcrBase::remake(). This is currently done in QcrComboBox::remake().
void QcrBase::remake()
{
    const QWidget* w = dynamic_cast<const QWidget*>(this);
    if ((w && w->isVisible()) || dynamic_cast<const QAction*>(this)) {
        remake_();
    }
}


//  ***********************************************************************************************
//! @class QcrRoot

QcrRoot::QcrRoot()
    : QcrBase {"root"}
{
    gRoot = this;
}

static int remakeLoops {0};

void QcrRoot::remakeAll()
{
    ++remakeLoops;
    if (remakeLoops>1)
        qFatal("BUG: circular remakeAll");
    remake();
    for (QWidget* w: findChildren<QWidget*>()) {
        if (w) {
            if (QcrBase* m = dynamic_cast<QcrBase*>(w)) {
                m->remake();
            }
        }
    }
    --remakeLoops;
}


//  ***********************************************************************************************
//! @class QcrRegistered

QcrRegistered::QcrRegistered(const QString& name)
    : QcrBase {gConsole->learn(name, this)} // console may change name (expand macros)
{}

//! Returns true if the value of *this is not to be stored as part of the QSettings.
bool QcrRegistered::adhoc() const
{
    return name().left(6)=="adhoc_";
}

QcrRegistered::~QcrRegistered()
{
    gConsole->forget(name());
}


//  ***********************************************************************************************
//! @class QcrModelessDialog (= persistent spawned popup)

QcrModelessDialog::QcrModelessDialog(QWidget* parent, const QString& name)
    : QcrRegistered {name}
    , QDialog {parent}
{
    setModal(false);
}

void QcrModelessDialog::closeEvent(QCloseEvent* event)
{
    QcrRegistered::deleteLater();
}

void QcrModelessDialog::setFromCommand(const QString& arg)
{
    if (arg!="close")
        throw QcrException("Unexpected command in ModelessDialog "+name());
    close();
}
