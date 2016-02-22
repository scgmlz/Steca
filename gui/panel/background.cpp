#include "background.h"
#include "mainwin.h"
#include "session.h"

namespace panel {

Background::Background(MainWin& mainWin,Session& session)
: super("Background",mainWin,session,Qt::Horizontal) {

  box->addWidget(iconButton(mainWin.actBackgroundBackground));
  box->addWidget(label("Degree:"));
  box->addWidget((spinDegree = spinCell(4,0,99)));
  box->addStretch();
  box->addWidget(iconButton(mainWin.actBackgroundEye));

  connect(spinDegree, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int degree) {
    getSession().setBackgroundPolynomDegree(degree);
  });
}

}

// eof
