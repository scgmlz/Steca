#include "app.h"
#include "../manifest.h"
#include "mainwin.h"
#include <QStyleFactory>
#include <iostream>

#include "core.h"

CoreProxy::CoreProxy(): core(new Core) {
}

CoreProxy::~CoreProxy(){
  delete core;
}

void CoreProxy::addFile(rcstr fileName) {
  core->addFile(fileName);
  emit filesChanged();
}

void CoreProxy::addFiles(str_lst fileNames) {
  for (auto& fileName: fileNames)
    core->addFile(fileName);
  emit filesChanged();
}

bool CoreProxy::hasFile(rcstr fileName) {
  return core->hasFile(fileName);
}

uint CoreProxy::numFiles(bool withCorrection) {
  return core->numFiles()
      + (withCorrection && hasCorrectionFile() ? 1 : 0);
}

str CoreProxy::fileName(uint i) {
  return core->fileName(i);
}

void CoreProxy::removeFile(uint i) {
  core->removeFile(i);
  emit filesChanged();
}

bool CoreProxy::hasCorrectionFile() {
  return core->hasCorrectionFile();
}

void CoreProxy::setCorrectionFile(rcstr fileName) {
  core->setCorrectionFile(fileName);
  emit filesChanged();
}

str CoreProxy::correctionFileName() {
  return core->correctionFileName();
}

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

#ifdef DEVEL
  coreProxy.addFiles({"/Users/igb/tmp/a","/Users/igb/tmp/b"});
  coreProxy.setCorrectionFile("/Users/igb/tmp/c");
#endif

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
  } catch(std::exception const& e) {
    qCritical("Error: %s", e.what());
  }

  return false;
}

// eof
