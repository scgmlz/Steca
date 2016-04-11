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
#include "thehub.h"
#include "core_session.h"
#include "core_dataset.h"
#include <QDate>

namespace io {
//------------------------------------------------------------------------------

class OutPoleFiguresParams: public panel::BoxPanel {
  SUPER(OutPoleFiguresParams,panel::BoxPanel)
public:
  OutPoleFiguresParams(TheHub&);
  
private:
  QSpinBox *spinAlpha,        *spinBeta,
           *spinCenterRadius, *spinCenterSearchRadius, 
           *spinSearchRadius, *spinIntensTreshold;
  QCheckBox *checkCenterInterpolation;
  
  static int const MAX_ALPHA                = 100;
  static int const MAX_BETA                 = 100;
  static int const MAX_CENTER_RADIUS        = 100;
  static int const MAX_CENTER_SEARCH_RADIUS = 100;
  static int const MAX_SEARCH_RADIUS        = 100;
  static int const MAX_INTENS_TRESHOLD      = 100;
  
};

OutPoleFiguresParams::OutPoleFiguresParams(TheHub& theHub): super("", theHub, Qt::Vertical) {
  auto gl1 = gridLayout();
  auto gl2 = gridLayout();
  box->addLayout(gl1);
  box->addWidget(checkCenterInterpolation = check("Polefigure center interpolation"));
  box->addLayout(gl2);
    
  gl1->addWidget(label("Alpha step"),                 0,0);
  gl1->addWidget(spinAlpha = spinCell(4,0,MAX_ALPHA), 0,1);
  gl1->addWidget(label("Beta step"),                  0,2);
  gl1->addWidget(spinBeta  = spinCell(4,0,MAX_BETA),  0,3);
  gl1->setColumnStretch(4,1);

  
  gl2->addWidget(label("Center radius"),                                          2,0);
  gl2->addWidget(spinCenterRadius       = spinCell(4,0,MAX_CENTER_RADIUS),        2,1);
  gl2->addWidget(label("Center search radius"),                                   2,3);
  gl2->addWidget(spinCenterSearchRadius = spinCell(4,0,MAX_CENTER_SEARCH_RADIUS), 2,4);

  gl2->addWidget(label("Search radius"),                                 3,0);
  gl2->addWidget(spinSearchRadius   = spinCell(4,0,MAX_CENTER_RADIUS),   3,1);
  gl2->addWidget(label("Intensity treshold"),                            3,3);
  gl2->addWidget(spinIntensTreshold = spinCell(4,0,MAX_INTENS_TRESHOLD), 3,4);
  gl2->setColumnStretch(5,1);

}

//------------------------------------------------------------------------------

OutPoleFigures::OutPoleFigures(rcstr title,TheHub& theHub_,QWidget* parent)
: super(title,parent), theHub(theHub_) {
  // RUNTIME_CHECK(theHub.session->numFiles(false) > 0, "No files added to session");

  setWidgets(
      new OutPoleFiguresParams(theHub),
      new OutTableWidget(theHub,
          {"date","value","comment","value"},
          {cmp_date,cmp_int,cmp_str,
           cmp_real
           })
  );
}

void OutPoleFigures::calculate() {
  // test
  // TODO complete
  auto &table = tableWidget->table();
  table.clear();
  table.addRow({ QDate::currentDate(),9,"a",2.1f});
  table.addRow({ QDate::currentDate(),9,"a",2.1f}); 
  table.addRow({ QDate::currentDate(),9,"a",2.1f});  
  table.addRow({ QDate::currentDate(),9,"a",2.1f});
  table.addRow({ QDate::currentDate(),9,"a",2.1f});
  table.addRow({ QDate::currentDate(),9,"a",2.1f}); 
  table.addRow({ QDate::currentDate(),9,"a",2.1f});  
  table.addRow({ QDate::currentDate(),9,"a",2.1f});
  table.addRow({ QDate::currentDate(),9,"a",2.1f});
  table.addRow({ QDate::currentDate(),9,"a",2.1f}); 
  table.addRow({ QDate::currentDate(),9,"a",2.1f});  
  table.addRow({ QDate::currentDate(),9,"a",2.1f});
  table.addRow({ QDate::currentDate(),9,"a",2.1f});
  table.addRow({ QDate::currentDate(),9,"a",2.1f}); 
  table.addRow({ QDate::currentDate(),9,"a",2.1f});  
  table.addRow({ QDate::currentDate(),9,"a",2.1f});
  table.addRow({ QDate::currentDate(),9,"a",2.1f});
  table.addRow({ QDate::currentDate(),9,"a",2.1f}); 
  table.addRow({ QDate::currentDate(),9,"a",2.1f});  
  table.addRow({ QDate::currentDate(),9,"a",2.1f});
  table.addRow({ QDate::currentDate(),9,"a",2.1f});
  table.addRow({ QDate::currentDate(),9,"a",2.1f}); 
  table.addRow({ QDate::currentDate(),9,"a",2.1f});  
  table.addRow({ QDate::currentDate(),9,"a",2.1f});
  table.addRow({ QDate::currentDate(),9,"a",2.1f});
  table.addRow({ QDate::currentDate(),9,"a",2.1f}); 
  table.addRow({ QDate::currentDate(),9,"a",2.1f});  
  table.addRow({ QDate::currentDate(),9,"a",2.1f});
  table.addRow({ QDate::currentDate(),9,"a",2.1f});
  table.addRow({ QDate::currentDate(),9,"a",2.1f}); 
  table.addRow({ QDate::currentDate(),9,"a",2.1f});  
  table.addRow({ QDate::currentDate(),9,"a",2.1f}); 
}

//------------------------------------------------------------------------------
}
// eof
