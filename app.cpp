#include "app.h"
#include <iostream>

App::App(int &argc, char *argv[])
: super(argc,argv)
{
  setApplicationName("STeCa2"); setApplicationVersion("0.1");
// TODO setOrganizationName();  setOrganizationDomain();
}

static QtMessageHandler oldHandler;

static void messageHandler(QtMsgType type, QMessageLogContext const& ctx, rcstr s) {
  switch (type) {
  case QtDebugMsg:
    std::cerr << "TR " << s.toStdString()
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

bool App::init() {
  oldHandler = qInstallMessageHandler(messageHandler);
  return true;
}

void App::done() {
  qInstallMessageHandler(nullptr);
}

bool App::notify(QObject* receiver, QEvent* event) {
  try {
    return super::notify(receiver, event);
  } catch(CriticalError& e) {
    qCritical("%s", e.what());
  } catch(std::exception& e) {
    qCritical("Error: %s", e.what());
  }

  return false;
}

// eof
