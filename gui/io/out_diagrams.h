// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      out_diagrams.h
//! @brief     Output of diagrams.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef OUT_DIAGRAMS_H
#define OUT_DIAGRAMS_H

#include "core_reflection_info.h"
#include "out_table.h"
#include "panels/panel.h"
#include "types/core_types_fwd.h"

namespace gui { namespace io {

//------------------------------------------------------------------------------

class OutDiagramsTable : public OutTable {
  SUPER(OutDiagramsTable,OutTable)
public:
  using super::super;
};

//------------------------------------------------------------------------------

class OutDiagramsTableWidget : public OutTableWidget {
  SUPER(OutDiagramsTableWidget,OutTableWidget)
public:
  using super::super;
};

//------------------------------------------------------------------------------

class SaveDiagramsWidget : public SaveOutputWidget {
  SUPER(SaveDiagramsWidget, SaveOutputWidget)
public:
  SaveDiagramsWidget();

  QRadioButton *currentDiagram_, *allData_;
  QComboBox    *fileTypes_;
};

//------------------------------------------------------------------------------

class OutDiagramsParams;
class DiagramsWidget;

class OutDiagramsWindow : public OutWindow {
  SUPER(OutDiagramsWindow, OutWindow)
public:
  OutDiagramsWindow(TheHub&, rcstr title, QWidget*);

  void calculate();
  void plot();
  bool saveDiagramOutput();
  void writeCurrentDiagramOutputFile(str filePath, str separator, str fileTag);
  void writeAllDataOutputFile(str filePath, str separator, str fileTag);

private:
  OutDiagramsParams      *params_;
  OutDiagramsTableWidget *tableData_;
  SaveDiagramsWidget     *saveWidget_;
  DiagramsWidget         *diagramsWidget_;
  core::ReflectionInfos   reflInfos_;
};

//------------------------------------------------------------------------------

}}
#endif  // OUT_Diagrams_H
