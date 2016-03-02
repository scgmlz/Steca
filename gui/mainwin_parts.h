/** \file
 */

#ifndef MAINWIN_PARTS_H
#define MAINWIN_PARTS_H

#include "gui_helpers.h"
#include "panel/panel.h"

class TheHub;

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

  FileView(TheHub&);

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
  DockFiles(TheHub&);
private:
  FileView *fileView;
};

//------------------------------------------------------------------------------

class DatasetView: public TreeListView {
  SUPER(DatasetView,TreeListView)
public:
  using Model = model::DatasetViewModel;

  DatasetView(TheHub&);

protected:
  void selectionChanged(QItemSelection const&, QItemSelection const&);

private:
  TheHub &theHub;
  Model  &model;
};

class DockDatasets: public DockWidget {
  SUPER(DockDatasets,DockWidget)
public:
  DockDatasets(TheHub&);
private:
  DatasetView *datasetView;
};

//------------------------------------------------------------------------------

class SplitImage: public BoxWidget {
  SUPER(SplitImage,BoxWidget)
public:
  SplitImage(TheHub&);
};

//------------------------------------------------------------------------------

class SplitFitting: public BoxWidget {
  SUPER(SplitFitting,BoxWidget)
public:
  SplitFitting(TheHub&);
};

//------------------------------------------------------------------------------

class SplitDiffractogram: public BoxWidget {
  SUPER(SplitDiffractogram,BoxWidget)
public:
  SplitDiffractogram(TheHub&);
};

//------------------------------------------------------------------------------

class DockDatasetInfo: public DockWidget {
  SUPER(DockDatasetInfo,DockWidget)
public:
  DockDatasetInfo(TheHub&);

private:
  class Info: public QWidget {
    SUPER(Info,QWidget)
  public:
    Info(InfoItems&);
    QGridLayout *grid;
  };

  TheHub &theHub;
  Info *info;
  InfoItems infoItems;
};

//------------------------------------------------------------------------------
}
#endif
