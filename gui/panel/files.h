/** \file
 */

#ifndef FILES_H
#define FILES_H

#include "panel.h"

namespace model {
class FileViewModel;
}

namespace panel {

class FileView: public ListView {
  SUPER(FileView,ListView)
public:
  using Model = model::FileViewModel;

  FileView(Model&);

protected:
  void selectionChanged(QItemSelection const&, QItemSelection const&);

public:
  void removeSelectedFile();

private:
  Model &model;
};

class Files: public BoxPanel {
  SUPER(Files,BoxPanel)
public:
  Files(MainWin&,Session&);

private:
  FileView *fileView;
};

}

#endif
