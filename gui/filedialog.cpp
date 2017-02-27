/*******************************************************************************
 * STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#include "filedialog.h"
#include "io_io.h"

#include <QFileDialog>
#include <QApplication>
#include <QAbstractProxyModel>
#include <QSortFilterProxyModel>
#include <QFileSystemModel>

namespace gui { namespace file_dialog {
//------------------------------------------------------------------------------

//typedef QModelIndex idx;
typedef QModelIndex const& rcidx;

class ProxyModel : public QSortFilterProxyModel {
  CLASS(ProxyModel) SUPER(QSortFilterProxyModel)
public:
  int columnCount(rcidx) const;
  QVariant headerData(int, Qt::Orientation, int = Qt::DisplayRole) const;
  QVariant data(rcidx, int = Qt::DisplayRole) const;
};

int ProxyModel::columnCount(rcidx) const {
  return 2;
}

QVariant ProxyModel::headerData(int section, Qt::Orientation o, int role) const {
  if (1 == section && Qt::Horizontal == o && role == Qt::DisplayRole)
    return "Comment";
  return super::headerData(section, o, role);
}

QVariant ProxyModel::data(rcidx idx, int role) const {
  if (idx.isValid() && 1 == idx.column()) {
    if (Qt::DisplayRole == role) {
      QFileSystemModel* fileModel = qobject_cast<QFileSystemModel*>(sourceModel());
      auto ix0 = fileModel->index(mapToSource(idx).row(), 0, mapToSource(idx.parent()));
      QFileInfo info(fileModel->rootDirectory().filePath(fileModel->fileName(ix0)));
      if (info.isFile()) {
        if (io::couldBeCaress(info))
          return "(caress) " + io::loadCaressComment(info.absoluteFilePath());
        else if (io::couldBeMar(info))
          return "(mar) ";
        else if (io::couldBeTiffDat(info))
          return "(tiff) ";
      }
    }

    return QVariant();
  }

  return super::data(idx, role);
}

//------------------------------------------------------------------------------

str openFileName(QWidget* parent, rcstr caption, rcstr dir, rcstr filter) {
  QFileDialog dlg(parent, caption, dir, filter);

  dlg.setOption(QFileDialog::DontUseNativeDialog);
  dlg.setViewMode(QFileDialog::Detail);
  dlg.setFileMode(QFileDialog::ExistingFile);
  dlg.setAcceptMode(QFileDialog::AcceptOpen);
  dlg.setReadOnly(true);

  dlg.setProxyModel(new ProxyModel);

  str fileName;
  if (dlg.exec() && !dlg.selectedFiles().isEmpty())
    fileName = dlg.selectedFiles().first();

  return fileName;
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

str_lst openFileNames(QWidget* parent, rcstr caption, rcstr dir, rcstr filter) {
  QFileDialog dlg(parent, caption, dir, filter);

  dlg.setOption(QFileDialog::DontUseNativeDialog);
  dlg.setViewMode(QFileDialog::Detail);
  dlg.setFileMode(QFileDialog::ExistingFiles);
  dlg.setAcceptMode(QFileDialog::AcceptOpen);
  dlg.setReadOnly(true);

  dlg.setProxyModel(new ProxyModel);

  str_lst fileNames;
  if (dlg.exec())
    fileNames = dlg.selectedFiles();

  return fileNames;
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
}}
// eof
