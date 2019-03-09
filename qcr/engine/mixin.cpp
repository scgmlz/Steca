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
#include <QMainWindow>

QcrRoot* gRoot {nullptr};

//  ***********************************************************************************************
//! @class QcrBase

QcrBase::QcrBase(const QString& name)
    : name_{name}
{}

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
    : QcrBase{"root"}
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
    auto* mw = dynamic_cast<QMainWindow*>(this);
    ASSERT(mw);
    for (QWidget* w: mw->findChildren<QWidget*>()) {
        if (w) {
            if (QcrBase* m = dynamic_cast<QcrBase*>(w)) {
                m->remake();
            }
        }
    }
    --remakeLoops;
}


//  ***********************************************************************************************
//! @class QcrCommandable

QcrCommandable::QcrCommandable(const QString& name)
    : QcrBase{name}
{}

//! Returns true if the value of *this is not to be stored as part of the QSettings.
bool QcrCommandable::adhoc() const
{
    return name().left(6)=="adhoc_";
}


//  ***********************************************************************************************
//! @class QcrRegistered

QcrRegistered::QcrRegistered(const QString& name)
    : QcrCommandable {gConsole->learn(name, this)} // console may change name (expand macros)
{}

QcrRegistered::~QcrRegistered()
{
    gConsole->forget(name());
}


//  ***********************************************************************************************
//! @class QcrModelessDialog (= persistent spawned popup)

QcrModelessDialog::QcrModelessDialog(QWidget* parent, const QString& name)
    : QcrRegistered{name}
    , QDialog {parent}
{
    setModal(false);
}

QcrModelessDialog::~QcrModelessDialog()
{
}

void QcrModelessDialog::closeEvent(QCloseEvent* event)
{
    deleteLater();
}

void QcrModelessDialog::setFromCommand(const QString& arg)
{
    if (arg!="close")
        throw QcrException{"Unexpected command in ModelessDialog "+name()};
    close();
}
