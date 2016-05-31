// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      app.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "app.h"
#include "../manifest.h"
#include "mainwin.h"
#include "types/core_async.h"
#include <QMessageBox>
#include <QStyleFactory>
#include <iostream>

//------------------------------------------------------------------------------

App::App(int& argc, char* argv[]) : super(argc, argv) {
  setApplicationName(APPLICATION_NAME);
  setApplicationVersion(APPLICATION_VERSION);
  setOrganizationName(ORGANIZATION_NAME);
  setOrganizationDomain(ORGANIZATION_DOMAIN);

#if defined(Q_OS_OSX)
  setStyle(QStyleFactory::create("Macintosh"));
#elif defined(Q_OS_WIN)
  setStyle(QStyleFactory::create("Fusion"));
#else
  setStyle(QStyleFactory::create("Fusion"));
#endif
}

static QtMessageHandler oldHandler;

static void messageHandler(QtMsgType type, QMessageLogContext const& ctx,
                           rcstr msg) {
  switch (type) {
  case QtDebugMsg:
    std::cerr << "TR " << msg.toStdString()  // TR for TRace
              << "\t[" << ctx.function << ']' << std::endl;
    break;
  case QtWarningMsg:
    QMessageBox::warning(QApplication::activeWindow(), qAppName(), msg);
    break;
  default:
    oldHandler(type, ctx, msg);
    break;
  }
}

static void waiting(bool on) {
  if (on)
    QApplication::setOverrideCursor(Qt::WaitCursor);
  else
    QApplication::restoreOverrideCursor();
}

int App::exec() {
  try {
    gui::MainWin mainWin;
    mainWin.show();

    oldHandler = qInstallMessageHandler(messageHandler);
    TakesLongTime::handler = waiting;

    int res = super::exec();

    TakesLongTime::handler = nullptr;
    qInstallMessageHandler(nullptr);

    return res;

  } catch (std::exception const& e) {
    qWarning("Fatal error: %s", e.what());
    return -1;
  }
}

bool App::notify(QObject* receiver, QEvent* event) {
  try {
    return super::notify(receiver, event);
  } catch (Exception const& e) {
    if (!e.silent())
      qWarning("%s", e.what());
  } catch (std::exception const& e) {
    qWarning("Error: %s", e.what());
  }

  return false;
}

//------------------------------------------------------------------------------
// eof
