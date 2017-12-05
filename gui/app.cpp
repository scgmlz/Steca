// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/app.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "app.h"
#include "../manifest.h"
#include "mainwin.h"
#include "typ/typ_log.h"

#include <QMessageBox>
#include <QStatusBar>
#include <QStyleFactory>
#include <iostream>

App::App(int& argc, char* argv[]) : super(argc, argv) {
  setApplicationName(APPLICATION_NAME);
  setApplicationVersion(
#include "../VERSION"
      );
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
static QAtomicInt       noWarning;

static void messageHandler(QtMsgType type, QMessageLogContext const& ctx,
                           rcstr msg) {
  switch (type) {
  case QtDebugMsg:
    std::cerr << "TR " << msg.toStdString()  // TR for TRace
              << "\t[" << ctx.function << ']' << std::endl;
    break;
  case QtWarningMsg:
    if (0 == noWarning)
      QMessageBox::warning(QApplication::activeWindow(), qAppName(), msg);
    break;
  default: oldHandler(type, ctx, msg); break;
  }
}

static void waiting(bool on) {
  if (on)
    QApplication::setOverrideCursor(Qt::WaitCursor);
  else
    QApplication::restoreOverrideCursor();
}

static QMainWindow* mainWindow;

static void logMessage(rcstr msg, MessageLogger::eType type) {
  EXPECT(mainWindow)

  str statusMsg;
  switch (type) {
  case MessageLogger::INFO: statusMsg = msg; break;
  case MessageLogger::POPUP:
    QMessageBox::information(mainWindow, EMPTY_STR, msg);
  case MessageLogger::WARN: statusMsg = "** " + msg + " **"; break;
  }

  mainWindow->statusBar()->showMessage(statusMsg, 3000);
}

int App::exec() {
  try {
    gui::MainWin mainWin;
    mainWin.show();

    oldHandler = qInstallMessageHandler(messageHandler);

    TakesLongTime::handler = waiting;

    mainWindow             = &mainWin;
    MessageLogger::handler = logMessage;

    int res = super::exec();

    MessageLogger::handler = nullptr;

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
    if (!e.silent()) qWarning("%s", e.what());
  } catch (std::exception const& e) { qWarning("Error: %s", e.what()); }

  return false;
}

NoWarnings::NoWarnings() {
  ++noWarning;
}

NoWarnings::~NoWarnings() {
  --noWarning;
}
