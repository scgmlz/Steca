// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/popup/filedialog.cpp
//! @brief     Implements functions openFileName(s), saveFileName, saveDirName in ns file_dialog
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "io/io_io.h"
#include <QFileDialog>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>

namespace {

typedef QModelIndex const& rcidx;

// ************************************************************************** //
//  class OpenFileProxyModel (file scope)
// ************************************************************************** //

class OpenFileProxyModel : public QSortFilterProxyModel {
public:
    int columnCount(rcidx) const;
    QVariant headerData(int, Qt::Orientation, int = Qt::DisplayRole) const;
    QVariant data(rcidx, int = Qt::DisplayRole) const;

private:
    mutable QHash<str, str> memInfo;
};

int OpenFileProxyModel::columnCount(rcidx) const {
    return 2;
}

QVariant OpenFileProxyModel::headerData(int section, Qt::Orientation ori, int role) const {
    if (1 == section && Qt::Horizontal == ori && role == Qt::DisplayRole)
        return "Comment";
    return QSortFilterProxyModel::headerData(section, ori, role);
}

QVariant OpenFileProxyModel::data(rcidx idx, int role) const {
    if (idx.isValid() && 1 == idx.column()) {
        if (Qt::DisplayRole == role) {
            QFileSystemModel* fileModel = qobject_cast<QFileSystemModel*>(sourceModel());
            auto ix0 = fileModel->index(mapToSource(idx).row(), 0, mapToSource(idx.parent()));
            QFileInfo info(fileModel->rootDirectory().filePath(fileModel->fileName(ix0)));
            if (info.isFile()) {
                auto path = info.absoluteFilePath();
                auto it = memInfo.find(path);
                if (memInfo.end() != it)
                    return *it;
                str loadInfo = io::loadComment(info);
                memInfo.insert(path, loadInfo);
                return loadInfo;
            }
        }
        return QVariant();
    }
    return QSortFilterProxyModel::data(idx, role);
}

} // anonymous namespace

// ************************************************************************** //
//  exported functions
// ************************************************************************** //


namespace file_dialog {

QStringList openFileNames(QWidget* parent, rcstr caption, rcstr dir, rcstr filter, bool plural) {
    QFileDialog dlg(parent, caption, dir, filter);
    dlg.setOption(QFileDialog::DontUseNativeDialog);
    dlg.setViewMode(QFileDialog::Detail);
    dlg.setAcceptMode(QFileDialog::AcceptOpen);
    dlg.setReadOnly(true);
    dlg.setProxyModel(new OpenFileProxyModel);
    dlg.setFileMode(plural ? QFileDialog::ExistingFiles : QFileDialog::ExistingFile);
    if (dlg.exec())
        return dlg.selectedFiles();
    return {};
}

str openFileName(QWidget* parent, rcstr caption, rcstr dir, rcstr filter) {
    QStringList fileNames = openFileNames(parent, caption, dir, filter, false);
    if (fileNames.isEmpty())
        return "";
    return fileNames.first();
}

str saveFileName(QWidget* parent, rcstr caption, rcstr dir, rcstr filter) {
    QFileDialog dlg(parent, caption, dir, filter);

    dlg.setOption(QFileDialog::DontUseNativeDialog);
    dlg.setViewMode(QFileDialog::Detail);
    dlg.setFileMode(QFileDialog::AnyFile);
    dlg.setAcceptMode(QFileDialog::AcceptSave);
    dlg.setConfirmOverwrite(false);

    str fileName;
    if (dlg.exec() && !dlg.selectedFiles().isEmpty())
        fileName = dlg.selectedFiles().first();

    return fileName;
}

str saveDirName(QWidget* parent, rcstr caption, rcstr dir) {
    QFileDialog dlg(parent, caption, dir);

    dlg.setOption(QFileDialog::DontUseNativeDialog);
    dlg.setViewMode(QFileDialog::Detail);
    dlg.setFileMode(QFileDialog::Directory);
    dlg.setAcceptMode(QFileDialog::AcceptSave);
    dlg.setConfirmOverwrite(false);

    str dirName;
    if (dlg.exec() && !dlg.selectedFiles().isEmpty())
        dirName = dlg.selectedFiles().first();

    return dirName;
}

} // namespace file_dialog

