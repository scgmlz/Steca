#ifndef PANEL_FILES_H
#define PANEL_FILES_H

#include "panel.h"
#include <QListView>
#include <QStyledItemDelegate>

class FileList: public QListView {
  SUPER(FileList,QListView) Q_OBJECT
public:
  FileList(Core&);

signals:
  void selectedFile(str fileName); // fileName may be empty (no file)

protected:
  void selectionChanged(QItemSelection const&, QItemSelection const&);
  void currentChanged(QModelIndex const&, QModelIndex const&);

private:
  class Model: public QAbstractListModel {
    SUPER(Model,QAbstractListModel)
  public:
    Model(Core&);

    int rowCount(QModelIndex const&)      const;
    QVariant data(QModelIndex const&,int) const;

  private:
    Core &core;
  };

  class Delegate: public QStyledItemDelegate {
    SUPER(Delegate,QStyledItemDelegate)
  public:
    Delegate(FileList&);

    void paint(QPainter*, QStyleOptionViewItem const&, QModelIndex const&) const;

  private:
    FileList &fileList;
  };

  Model    model;
  Delegate delegate;
};

class PanelFiles: public Panel {
  SUPER(PanelFiles,Panel)
public:
  PanelFiles(MainWin&);

private:
  FileList *fileList;
  Button *btnAddCorrectionFile, *btnAddFile, *btnRemoveFile;
};

#endif
