#ifndef PANEL_FILES_H
#define PANEL_FILES_H

#include "panel.h"
#include <QListView>
#include <QStyledItemDelegate>

class CoreProxy;

class FileList: public QListView {
  SUPER(FileList,QListView) Q_OBJECT
public:
  FileList(CoreProxy&);

protected:
  void selectionChanged(QItemSelection const&, QItemSelection const&);
signals:
  void selectedFile(str fileName); // fileName may be empty (no file)
public:
  void removeSelectedFile();

private:
  class Model: public QAbstractListModel {
    SUPER(Model,QAbstractListModel)
  public:
    Model(CoreProxy&);

    int rowCount(QModelIndex const&)      const;
    QVariant data(QModelIndex const&,int) const;

  private:
    CoreProxy &coreProxy;
  };

  class Delegate: public QStyledItemDelegate {
    SUPER(Delegate,QStyledItemDelegate)
  public:
    Delegate();

    void paint(QPainter*, QStyleOptionViewItem const&, QModelIndex const&) const;
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
  PushButton *btnAddCorrectionFile;
  IconButton *btnAddFiles, *btnRemoveFile;
};

#endif
