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

QcrModal::~QcrModal()
{
    gLogger->log((result() ? "@accept " : "@reject")+name());
    gConsole->closeModalDialog(name());
}


//  ***********************************************************************************************
//! @class QcrModalDialog

QcrModalDialog::QcrModalDialog(QWidget* parent, const QString& caption)
    : QcrModal{"modal"}
    , QDialog{parent}
{
    setWindowTitle(caption);
}

int QcrModalDialog::exec()
{
    if (gConsole->hasCommandsOnStack()) {
        open();
        gConsole->commandsFromStack(); // returns upon command "@close"
        close();
        return result();
    } else {
        connect(gConsole, &Console::closeDialog, [this](){accept();});
        return QDialog::exec();
    }
}

void QcrModalDialog::setFromCommand(const QString& arg)
{
    if (arg=="")
        throw QcrException{"Empty argument in Dialog command"};
    if (arg=="close") {
        accept();
        return;
    }
}


//  ***********************************************************************************************
//! @class QcrFileDialog

QcrFileDialog::QcrFileDialog(
    QWidget* parent, const QString& caption, const QString& directory, const QString& filter)
    : QcrModal{"fdia"}
    , QFileDialog{parent, caption, directory, filter}
{}

QcrFileDialog::~QcrFileDialog()
{
    gLogger->log("fdia select "+selectedFiles().join(';'));
}

int QcrFileDialog::exec()
{
    connect(gConsole, &Console::closeDialog,
            [this](bool ok){ if (ok) accept(); else reject();});
    if (gConsole->hasCommandsOnStack()) {
        open();
        gConsole->commandsFromStack(); // returns upon command "@accept" or "@reject"
    } else {
        QDialog::exec();
    }
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
