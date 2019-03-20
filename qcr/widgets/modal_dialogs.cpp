//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/widgets/modal_dialogs.cpp
//! @brief     Implements classes QcrModal, QcrModalDialog, QcrFileDialog
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#include "qcr/widgets/modal_dialogs.h"
#include "qcr/base/debug.h"
#include "qcr/base/qcrexception.h"
#include "qcr/engine/console.h"
#include "qcr/engine/logger.h"


//  ***********************************************************************************************
//! @class QcrModal

QcrModal::QcrModal(const QString& name)
    : QcrCommandable{name}
{
    gConsole->openModalDialog(name, this);
}

//! Called after destruction of a QDialog, this destructor pops a dialog from the command registry.

//! The mixin class QcrModal mainly exists for the sake of this destructor.
//! It calls gConsole at the very end of the destruction of a QcrDialog or QcrFileDialog.
//! This tear down order cannot be altered unpunished.
QcrModal::~QcrModal()
{
    gConsole->closeModalDialog(name());
}


//  ***********************************************************************************************
//! @class QcrModalDialog

QcrModalDialog::QcrModalDialog(const QString& name, QWidget* parent, const QString& caption)
    : QcrModal{name}
    , QDialog{parent}
{
    setWindowTitle(caption);
}

QcrModalDialog::~QcrModalDialog()
{
    gLogger->log((result() ? "@accept " : "@reject ")+name());
}

void QcrModalDialog::onClose(bool ok)
{
    // qDebug() << "dialog " << (long)this << " received close signal, ok=" << ok;
    if (ok)
        accept();
    else
        reject();
}

int QcrModalDialog::exec()
{
    connect(gConsole, &Console::closeDialog, [this](bool ok){ onClose(ok);});
    if (gConsole->hasCommandsOnStack()) {
        open();
        gConsole->commandsFromStack(); // returns after emitting signal closeDialog
    } else {
        QDialog::exec();
    }
    return result();
}

void QcrModalDialog::setFromCommand(const QString& arg)
{
    throw QcrException{"Unexpected argument '"+arg+"' in Dialog command"};
}


//  ***********************************************************************************************
//! @class QcrFileDialog

QcrFileDialog::QcrFileDialog(
    QWidget* parent, const QString& caption, const QString& directory, const QString& filter)
    : QcrModal{"fdia"}
    , QFileDialog{parent, caption, directory, filter}
{
    // qDebug() << "starting dialog " << (long)this;
}

QcrFileDialog::~QcrFileDialog()
{
    gLogger->log("fdia select "+selectedFiles().join(';'));
    gLogger->log((result() ? "@accept " : "@reject ")+name());
}

void QcrFileDialog::onClose(bool ok)
{
    if (ok)
        accept();
    else
        reject();
}

int QcrFileDialog::exec()
{
    connect(gConsole, &Console::closeDialog, this, &QcrFileDialog::onClose);
    if (gConsole->hasCommandsOnStack()) {
        open();
        gConsole->commandsFromStack(); // returns after emitting signal closeDialog
    } else {
        QDialog::exec();
    }
    disconnect(gConsole, &Console::closeDialog, this, &QcrFileDialog::onClose);
    return result();
}

void QcrFileDialog::setFromCommand(const QString& arg)
{
    if (arg=="")
        throw QcrException{"Empty argument in FileDialog command"};
    QStringList args = arg.split(' ');
    if (args[0]!="select")
        throw QcrException{"Unexpected filedialog command"};
    if (args.size()<2)
        throw QcrException{"Missing argument to command 'select'"};
    QStringList list = args[1].split(';');
    selectFile('"' + list.join("\" \"") + '"');
}
