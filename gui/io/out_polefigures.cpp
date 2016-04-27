// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      out_polefigures.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "out_polefigures.h"
#include "out_table.h"
#include "panels/panel.h"
#include <QDate>

#ifdef STECA_LABS
#include "gl/gl_canvas.h"
#endif

namespace gui { namespace io {
//------------------------------------------------------------------------------

class OutPoleFiguresParams: public panel::BoxPanel {
  SUPER(OutPoleFiguresParams,panel::BoxPanel)
public:
  OutPoleFiguresParams(TheHub&);
};

OutPoleFiguresParams::OutPoleFiguresParams(TheHub& hub): super("", hub, Qt::Horizontal) {
  box_->addWidget(label("Param"));
  box_->addWidget(editCell(16));
  box_->addStretch();
}

//------------------------------------------------------------------------------

OutPoleTabs::OutPoleTabs(TheHub& hub): super(hub) {
#ifdef STECA_LABS
  {
    auto &tab = addTab("Sphere");
  }
#endif
  {
    auto &tab = addTab("Table");
  }
  {
    auto &tab = addTab("Graph");
  }
}

//------------------------------------------------------------------------------

OutPoleFigures::OutPoleFigures(TheHub& hub,rcstr title,QWidget* parent)
: super(hub,title,parent) {
  auto *tabs = new OutPoleTabs(hub);
  setWidgets(new OutPoleFiguresParams(hub), tabs);

  tableWidget_ = new OutTableWidget(hub,
                      {"int","str","real","date"},
                      {cmp_int, cmp_str, cmp_real, cmp_date});
  tabs->tab(1).box->addWidget(tableWidget_);

#ifdef STECA_LABS
  auto *canvas = new gl::Canvas();
  tabs->tab(0).box->addWidget(canvas);
  auto *camera = new gl::Camera();
  canvas->setCamera(camera);
#endif
}

void OutPoleFigures::calculate() {
  // test
  // TODO complete
  auto &table = tableWidget_->table();
  table.clear();

  table.addRow({10,"d",10.10,QDate(2010,10,10),});
  table.addRow({20,"c",10.20,QDate(2020,10,10),});
  table.addRow({30,"b",10.30,QDate(2030,10,10),});
  table.addRow({40,"a",10.40,QDate(2040,10,10),});
}

//------------------------------------------------------------------------------
}}
// eof
