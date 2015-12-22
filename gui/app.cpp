#include "app.h"
#include "manifest.h"
#include <iostream>

App::App(int &argc, char *argv[])
  : super(argc,argv)
{
  setApplicationName(APPLICATION_NAME);
  setApplicationVersion(APPLICATION_VERSION);
  setOrganizationName(ORGANIZATION_NAME);
  setOrganizationDomain(ORGANIZATION_DOMAIN);
}

int App::exec() {
  init();
  int res = super::exec();
  done();
  return res;
}

static QtMessageHandler oldHandler;

static void messageHandler(QtMsgType type, QMessageLogContext const& ctx, rcstr s) {
  switch (type) {
  case QtDebugMsg:
    std::cerr << "TR " << s.toStdString() // TR for TRace
              << "\t[" << ctx.function << ']' << std::endl;
    break;
  case QtWarningMsg:
    warn(qApp, s);
    break;
  default:
    oldHandler(type,ctx,s);
    break;
  }
}

void App::init() {
  oldHandler = qInstallMessageHandler(messageHandler);
}

void App::done() {
  qInstallMessageHandler(nullptr);
}

bool App::notify(QObject* receiver, QEvent* event) {
  try {
    return super::notify(receiver, event);
  } catch(CriticalError const& e) {
    qCritical("CriticalError: %s", e.what());
  } catch(std::exception const& e) {
    qCritical("Error: %s", e.what());
  }

  return false;
}

// eof
