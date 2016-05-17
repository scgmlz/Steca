// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2 REVIEW
//
//! @file      out_polefigures.h
//! @brief     A dynamic table for pole figures.
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

#include "out_table.h"
#include "panels/panel.h"
#include "types/core_types_fwd.h"
#include "core_reflection_info.h"

namespace gui { namespace io {
//------------------------------------------------------------------------------

class OutPoleFiguresParams;
class PoleWidget;

class OutPoleFigures: public OutWindow {
  SUPER(OutPoleFigures,OutWindow)
public:
  OutPoleFigures(TheHub&,rcstr title,QWidget*);
  void calculate();

private:
  OutPoleFiguresParams  *params_;
  OutTableWidget        *tableData_;
  PoleWidget            *poleWidget_;
  core::ReflectionInfos  rs_;
};

//------------------------------------------------------------------------------
}}
#endif // OUT_POLEFIGURES_H
