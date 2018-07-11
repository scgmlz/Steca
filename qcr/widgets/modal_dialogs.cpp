//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/widgets/modal_dialogs.cpp
//! @brief     Implements functions that return new Qt objects
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#include "modal_dialogs.h"
#include "qcr/base/debug.h"
#include "qcr/base/qcrexception.h"
#include "qcr/base/string_ops.h"
#include "qcr/engine/console.h"


//  ***********************************************************************************************
//! @class QcrModal

QcrModal::QcrModal(QObject& object, const QString& name)
    : QcrSettable {object, "@push " + name}
{
}

QcrModal::~QcrModal()
{
    qDebug() << "~QcrModal1";
    gConsole->log("@close");
    qDebug() << "~QcrModal2";
    gConsole->call("@pop");
    qDebug() << "~QcrModal3";
}


//  ***********************************************************************************************
//! @class QcrDialog

QcrDialog::QcrDialog(QWidget* parent, const QString& caption)
    : QDialog {parent}
    , QcrModal {*this, "dlog"}
{
    setWindowTitle(caption);
}

QcrDialog::~QcrDialog()
{
    qDebug() << "~QcrDialog";
    gConsole->log("dlog closing");
}

int QcrDialog::exec()
{
    if (gConsole->hasCommandsOnStack()) {
        open();
        gConsole->commandsFromStack();
        close();
        return QDialog::Accepted;
    } else
        return QDialog::exec();
}

void QcrDialog::executeConsoleCommand(const QString& arg)
{
    if (arg=="")
        throw QcrException("Empty argument in Dialog command");
    if (arg=="close") {
        accept();
        return;
    }
}


//  ***********************************************************************************************
//! @class QcrFileDialog

QcrFileDialog::QcrFileDialog(
    QWidget* parent, const QString& caption, const QString& directory, const QString& filter)
    : QFileDialog {parent, caption, directory, filter}
    , QcrModal {*this, "fdia"}
{}

QcrFileDialog::~QcrFileDialog()
{
    qDebug() << "~QcrFileDialog";
    gConsole->log("fdia select "+selectedFiles().join(';'));
}

int QcrFileDialog::exec()
{
    if (gConsole->hasCommandsOnStack()) {
        open();
        gConsole->commandsFromStack();
        close();
        return QDialog::Accepted;
    } else
        return QDialog::exec();
}

void QcrFileDialog::executeConsoleCommand(const QString& arg)
{
    if (arg=="")
        throw QcrException("Empty argument in FileDialog command");
    if (arg=="close") {
        accept();
        return;
    }
    QStringList args = arg.split(' ');
    if (args[0]!="select")
        throw QcrException("Unexpected filedialog command");
    if (args.size()<2)
        throw QcrException("Missing argument to command 'select'");
    QStringList list = args[1].split(';');
    QString tmp = '"' + list.join("\" \"") + '"';
    selectFile(tmp);
}
