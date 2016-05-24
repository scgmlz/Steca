// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      out_polefigures.h
//! @brief     Output of pole data.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef OUT_POLEFIGURES_H
#define OUT_POLEFIGURES_H

#include "core_reflection_info.h"
#include "out_table.h"
#include "panels/panel.h"
#include "types/core_types_fwd.h"

namespace gui { namespace io {

//------------------------------------------------------------------------------

class OutPoleFiguresTable : public OutTable {
  SUPER(OutPoleFiguresTable,OutTable)
public:
  using super::super;
};

//------------------------------------------------------------------------------

class OutPoleFiguresTableWidget : public OutTableWidget {
  SUPER(OutPoleFiguresTableWidget,OutTableWidget)
public:
  using super::super;
};

//------------------------------------------------------------------------------

class SavePoleFiguresWidget : public SaveOutputWidget {
  SUPER(SavePoleFiguresWidget, SaveOutputWidget)
public:
  SavePoleFiguresWidget();
  QCheckBox *outputInten_, *outputPeakPos_, *outputFWHM_;
  QRadioButton *selectedRefl_,*allRefl_;
  QGroupBox *gbRefl_, *gbInfos_;
};

//------------------------------------------------------------------------------

class OutPoleFiguresParams;
class PoleWidget;

class OutPoleFiguresWindow : public OutWindow {
  SUPER(OutPoleFiguresWindow, OutWindow)
public:
  OutPoleFiguresWindow(TheHub&, rcstr title, QWidget*);
  void calculate();
  void display(int);
  void savePoleFigureOutput();
  void writePoleFigureOutputFile(int index);

private:
  OutPoleFiguresParams          *params_;
  OutPoleFiguresTableWidget     *tableData_;
  PoleWidget                    *poleWidget_;
  SavePoleFiguresWidget         *saveWidget_;
  QVector<core::ReflectionInfos> reflInfos_;
};

//------------------------------------------------------------------------------

}}
#endif  // OUT_POLEFIGURES_H
