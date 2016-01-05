#ifndef CORE_H
#define CORE_H

#include <QtGlobal>
#include <QString>
#include <QStringBuilder>
#include <QException>

typedef QString     str;
typedef str const&  rcstr;

#ifdef BUILDING_LIBRARY
#define QSHARED_EXPORT Q_DECL_EXPORT
#else
#define QSHARED_EXPORT Q_DECL_IMPORT
#endif

#include <QVector>
#include <QFileInfo>

namespace core {

class QSHARED_EXPORT Exception: public QException {
public:
  Exception(rcstr msg_): msg(msg_) {}

  const char* what() const throw() {
    return msg.toLocal8Bit().data();
  }

  str msg;
};

void raiseError(rcstr msg) throw(Exception);

class QSHARED_EXPORT Session final {
public:

  class QSHARED_EXPORT File final {
  public:
    File();
    File(rcstr fileName);

    str name() const { return info.fileName(); }

    void load() throw(Exception);

  private:
    friend class Session;
    QFileInfo info;
  };

  class QSHARED_EXPORT Files final: public QVector<File*> {
  public:
  };

public:
  Session();
 ~Session();

  void addFile(rcstr fileName) throw(Exception);
  bool hasFile(rcstr fileName);
  void remFile(uint i);

  bool hasCorrFile()   const;
  void setCorrFile(rcstr fileName); // fileName may be empty -> unsets

  Files const& getDataFiles() const { return dataFiles; }
  File  const& getCorrFile()  const { return corrFile; }

private:
  Files dataFiles;
  File  corrFile;
};

}

#endif
