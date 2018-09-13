//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/file_dialog.cpp
//! @brief     Implements functions in namespace file_dialog
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "file_dialog.h"
#include "core/loaders/loaders.h"
#include "qcr/base/debug.h" // qWarning
#include "qcr/base/string_ops.h"
#include "qcr/widgets/modal_dialogs.h"
#include <QFileSystemModel>
#include <QMessageBox>
#include <QSortFilterProxyModel>

namespace {

//  ***********************************************************************************************
//! @class OpenFileProxyModel for local use in queryImportFileNames.

class OpenFileProxyModel : public QSortFilterProxyModel {
public:
    int columnCount(const QModelIndex&) const { return 2; }
    QVariant headerData(int, Qt::Orientation, int = Qt::DisplayRole) const;
    QVariant data(const QModelIndex&, int = Qt::DisplayRole) const;
private:
    mutable QHash<QString, QString> memInfo;
};

QVariant OpenFileProxyModel::headerData(int section, Qt::Orientation ori, int role) const
{
    if (1 == section && Qt::Horizontal == ori && role == Qt::DisplayRole)
        return "Comment";
    return QSortFilterProxyModel::headerData(section, ori, role);
}

QVariant OpenFileProxyModel::data(const QModelIndex& idx, int role) const
{
    if (idx.isValid() && 1 == idx.column()) {
        if (Qt::DisplayRole == role) {
            QFileSystemModel* fileModel = qobject_cast<QFileSystemModel*>(sourceModel());
            QModelIndex ix0 =
                fileModel->index(mapToSource(idx).row(), 0, mapToSource(idx.parent()));
            QFileInfo info(fileModel->rootDirectory().filePath(fileModel->fileName(ix0)));
            if (info.isFile()) {
                const QString& path = info.absoluteFilePath();
                auto it = memInfo.find(path);
                if (it != memInfo.end())
                    return *it;
                QString loadInfo = load::loadComment(info);
                memInfo.insert(path, loadInfo);
                return loadInfo;
            }
        }
        return QVariant();
    }
    return QSortFilterProxyModel::data(idx, role);
}

//  ***********************************************************************************************
//! @class FileDialog for local use in Steca file dialogs. Manages default directory.

class FileDialog : public QcrFileDialog {
public:
    FileDialog(QWidget*, const QString&, QDir&, const QString& filter = QString());
    QStringList getFiles();
    QString getFile();
private:
    QDir& dir_;
};

FileDialog::FileDialog(QWidget* parent, const QString& caption, QDir& dir, const QString &filter)
    : QcrFileDialog(parent, caption, dir.absolutePath(), filter)
    , dir_(dir)
{
    setOption(QFileDialog::DontUseNativeDialog);
    setViewMode(QFileDialog::Detail);
    setConfirmOverwrite(false);
}

QStringList FileDialog::getFiles()
{
    QStringList ret = selectedFiles();
    if (!ret.isEmpty())
        dir_ = QFileInfo(ret.at(0)).absolutePath();
    return ret;
}

QString FileDialog::getFile()
{
    QStringList files = getFiles();
    if (files.isEmpty())
        return "";
    return files.first();
}

} // namespace

//  ***********************************************************************************************
//  exported functions
//  ***********************************************************************************************

namespace file_dialog {

static eFileOverwritePolicy fileOverwritePolicy = eFileOverwritePolicy::PROMPT;

void setFileOverwritePolicy(eFileOverwritePolicy val)
{
    fileOverwritePolicy = val;
    //qDebug() << "fileOverwritePolicy set to " << (int)fileOverwritePolicy << "\n";
}

bool confirmOverwrite(const QString& name, QWidget* parent, const QString& path)
{
    switch (fileOverwritePolicy) {
    case eFileOverwritePolicy::PROMPT:
        return QMessageBox::question(parent, "File exists", "Overwrite " + path + " ?")
                == QMessageBox::Yes;
        break;
    case eFileOverwritePolicy::PANIC: {
        QByteArray tmp = path.toLatin1();
        qFatal("attempting to write to existing file '%s'", tmp.constData());
        break;
    }
    case eFileOverwritePolicy::SILENT_OVERWRITE:
        return true;
        break;
    default:
        qFatal("impossible case");
        break;
    }
}

//! Opens file for writing; asks for confirmation before overwriting.
QFile* openFileConfirmOverwrite(const QString& name, QWidget* parent, const QString& path)
{
    QFile* ret = new QFile(path);
    if (ret->exists() &&
        !confirmOverwrite(name, parent, path)) {
        delete ret;
        return nullptr;
    } // else:
    if (!ret->open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file for writing: " << path;
        delete ret;
        return nullptr;
    }
    return ret;
}

//! Runs dialog that prompts for input files. Returns list of absolute paths. May change dir.
QStringList queryImportFileNames(
    QWidget* parent, const QString& caption, QDir& dir, const QString& filter, bool plural)
{
    FileDialog dlg(parent, caption, dir, filter);
    dlg.setAcceptMode(QFileDialog::AcceptOpen);
    dlg.setReadOnly(true);
    dlg.setProxyModel(new OpenFileProxyModel);
    dlg.setFileMode(plural ? QFileDialog::ExistingFiles : QFileDialog::ExistingFile);
    if (!dlg.exec())
        return {};
    return dlg.getFiles();
}

//! Runs dialog that prompts for one input file. Returns absolute path. May change dir.
QString queryImportFileName(
    QWidget* parent, const QString& caption, QDir& dir, const QString& filter)
{
    QStringList fileNames = queryImportFileNames(parent, caption, dir, filter, false);
    if (fileNames.isEmpty())
        return "";
    return fileNames.first();
}

//! Runs dialog that prompts for one output file. Returns absolute path. May change dir.
QString queryExportFileName(
    QWidget* parent, const QString& caption, QDir& dir, const QString& filter)
{
    FileDialog dlg(parent, caption, dir, filter);
    dlg.setFileMode(QFileDialog::AnyFile);
    dlg.setAcceptMode(QFileDialog::AcceptSave);
    if (!dlg.exec())
        return "";
    return dlg.getFile();
}

//! Runs dialog that prompts for a directory. Returns absolute directory path. May change dir.
// TODO return value VS dir ???
QString queryDirectory(QWidget* parent, const QString& caption, const QString& dirname)
{
    QDir dir(dirname);
    FileDialog dlg(parent, caption, dir);
    dlg.setFileMode(QFileDialog::Directory);
    dlg.setAcceptMode(QFileDialog::AcceptSave);
    if (!dlg.exec())
        return "";
    return dlg.getFile();
}

} // namespace file_dialog
