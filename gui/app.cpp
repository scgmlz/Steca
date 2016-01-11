#include "app.h"
#include "../manifest.h"
#include "mainwin.h"
#include <QStyleFactory>
#include <iostream>

//------------------------------------------------------------------------------

App *app;

App::App(int &argc, char *argv[])
  : super(argc,argv), mainWin(nullptr)
{
  setApplicationName(APPLICATION_NAME);
  setApplicationVersion(APPLICATION_VERSION);
  setOrganizationName(ORGANIZATION_NAME);
  setOrganizationDomain(ORGANIZATION_DOMAIN);

#if defined(Q_OS_OSX)
  setStyle(QStyleFactory::create("Macintosh"));
#elif defined(Q_OS_WIN)
  setStyle(QStyleFactory::create("Windows"));
#else
  setStyle(QStyleFactory::create("Fusion"));
#endif

  app = this;
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

int App::exec() {
  oldHandler = qInstallMessageHandler(messageHandler);

  mainWin = new MainWin;
  mainWin->show();

  int res = super::exec();
  delete mainWin; mainWin = nullptr;

  qInstallMessageHandler(nullptr);
  return res;
}

bool App::notify(QObject* receiver, QEvent* event) {
  try {
    return super::notify(receiver, event);
  } catch(CriticalError const& e) {
    qCritical("CriticalError: %s", e.what());
  } catch(Exception const& e) {
    qWarning("%s", e.msg.toLocal8Bit().constData());
  } catch(std::exception const& e) {
    qWarning("Error: %s", e.what());
  }

  return false;
}

// eof
