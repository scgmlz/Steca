// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/filedialog.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "filedialog.h"
#include "io_io.h"

#include <QAbstractProxyModel>
#include <QApplication>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>

namespace gui {
namespace file_dialog {
//------------------------------------------------------------------------------

// typedef QModelIndex idx;
typedef QModelIndex const& rcidx;

class OpenFileProxyModel : public QSortFilterProxyModel {
    CLASS(OpenFileProxyModel)
    SUPER(QSortFilterProxyModel) public : int columnCount(rcidx) const;
    QVariant headerData(int, Qt::Orientation, int = Qt::DisplayRole) const;
    QVariant data(rcidx, int = Qt::DisplayRole) const;

private:
    mutable QHash<str, str> memInfo;
};

int OpenFileProxyModel::columnCount(rcidx) const {
    return 2;
}

QVariant OpenFileProxyModel::headerData(int section, Qt::Orientation o, int role) const {
    if (1 == section && Qt::Horizontal == o && role == Qt::DisplayRole)
        return "Comment";
    return super::headerData(section, o, role);
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

                str loadInfo;
                if (io::couldBeCaress(info))
                    loadInfo = "[car] " + io::loadCaressComment(path);
                else if (io::couldBeMar(info))
                    loadInfo = "[mar] ";
                else if (io::couldBeTiffDat(info))
                    loadInfo = "[tif] ";

                memInfo.insert(path, loadInfo);
                return loadInfo;
            }
        }

        return QVariant();
    }

    return super::data(idx, role);
}

//------------------------------------------------------------------------------

class OpenFileDialog : public QFileDialog {
public:
    using QFileDialog::QFileDialog;
    void init();
};

void OpenFileDialog::init() {
    setOption(DontUseNativeDialog);
    setViewMode(Detail);
    setAcceptMode(AcceptOpen);
    setReadOnly(true);
    setProxyModel(new OpenFileProxyModel);
}

str openFileName(QWidget* parent, rcstr caption, rcstr dir, rcstr filter) {
    OpenFileDialog dlg(parent, caption, dir, filter);
    dlg.init();
    dlg.setFileMode(QFileDialog::ExistingFile);

    str fileName;
    if (dlg.exec() && !dlg.selectedFiles().isEmpty())
        fileName = dlg.selectedFiles().first();

    return fileName;
}

str_lst openFileNames(QWidget* parent, rcstr caption, rcstr dir, rcstr filter) {
    OpenFileDialog dlg(parent, caption, dir, filter);
    dlg.init();
    dlg.setFileMode(QFileDialog::ExistingFiles);

    str_lst fileNames;
    if (dlg.exec())
        fileNames = dlg.selectedFiles();

    return fileNames;
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

//------------------------------------------------------------------------------
}
}
// eof
