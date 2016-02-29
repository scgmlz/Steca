/** \file
 */

#ifndef MAINWIN_PARTS_H
#define MAINWIN_PARTS_H

#include "gui_helpers.h"
#include "panel/panel.h"

class MainWin; class Session;

namespace model {
class FileViewModel;
class DatasetViewModel;
}

namespace panel {
//------------------------------------------------------------------------------

class FileView: public ListView {
  SUPER(FileView,ListView)
public:
  using Model = model::FileViewModel;

  FileView(Model&);

protected:
  void selectionChanged(QItemSelection const&, QItemSelection const&);

public:
  void removeSelected();
  void update();

private:
  Model &model;
};

class DockFiles: public DockWidget {
  SUPER(DockFiles,DockWidget)
public:
  DockFiles(MainWin&,Session&);
private:
  FileView *fileView;
};

//------------------------------------------------------------------------------

class DatasetView: public TreeListView {
  SUPER(DatasetView,TreeListView)
public:
  using Model = model::DatasetViewModel;

  DatasetView(Model&);

protected:
  void selectionChanged(QItemSelection const&, QItemSelection const&);

private:
  Model &model;
};

class DockDatasets: public DockWidget {
  SUPER(DockDatasets,DockWidget)
public:
  DockDatasets(MainWin&,Session&);
private:
  DatasetView *datasetView;
};

//------------------------------------------------------------------------------

class SplitImage: public BoxWidget {
  SUPER(SplitImage,BoxWidget)
public:
  SplitImage(MainWin&,Session&);
};

//------------------------------------------------------------------------------

class SplitFitting: public BoxWidget {
  SUPER(SplitFitting,BoxWidget)
public:
  SplitFitting(MainWin&,Session&);
};

//------------------------------------------------------------------------------

class SplitDiffractogram: public BoxWidget {
  SUPER(SplitDiffractogram,BoxWidget)
public:
  SplitDiffractogram(MainWin&,Session&);
};

//------------------------------------------------------------------------------

class DockDatasetInfo: public DockWidget {
  SUPER(DockDatasetInfo,DockWidget)
public:
  DockDatasetInfo(MainWin&,Session&);

private:
  class Info: public QWidget {
    SUPER(Info,QWidget)
  public:
    Info(InfoItems&);
    QGridLayout *grid;
  };

  Info *info;
  InfoItems infoItems;
};

//------------------------------------------------------------------------------
}
#endif
