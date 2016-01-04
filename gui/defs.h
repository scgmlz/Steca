// common and useful definitions

#ifndef DEFS_H
#define DEFS_H

#define DEVEL

#include <QtGlobal>
#include <QException>

// strings
#include <QString>
#include <QStringBuilder>

typedef QString     str;
typedef str const&  rcstr;
typedef char const* pcstr;

typedef QStringList   str_lst;

// a class definition helper: this class and superclass access
#define SUPER(cls,sup)  typedef cls thisCls; typedef sup super;

// a class to with inherited constructors
#define INHERIT_SUPER(cls,ns,sup) SUPER(cls,ns::sup)  \
  public: using super::sup; private:

// iteration
#define for_i(num)          for (int i=0, iEnd=(num); i<iEnd; ++i)
#define for_i_reverse(num)  for (int i=(num); i-->0; )

// own pointer
template <typename T> struct own {
  own(T* p_): p(p_) {}
 ~own() { delete p; }

  T* p;
};

// exceptions

class Exc: public QException {
public:
  Exc(rcstr msg);
 ~Exc() throw ();

  str msg;

  void warn() const;
};

#define DECLARE_EXCEPTION(ExcCls)    \
class ExcCls: public Exc {          \
public:                             \
  ExcCls(rcstr msg): Exc(msg) {}    \
 ~ExcCls() throw ()           {}    \
  ExcCls* clone() const {           \
    return new ExcCls(*this);        \
  }                                 \
  void raise() const {              \
      throw *this;                  \
  }                                 \
};

DECLARE_EXCEPTION(CriticalError)
void CRITICAL_ERROR(rcstr msg) throw (CriticalError);

// debug support
#define ASSERT(cond)      Q_ASSERT(cond);

#ifndef QT_NO_DEBUG
#include <QDebug>
#define TR(what)          { qDebug() << what; } // TR: short for 'trace'
#define DEBUG_CODE(code)  code
#define VERIFY(cond)      Q_ASSERT(cond);
#else
#define TR(what)          { }
#define DEBUG_CODE(code)
#define VERIFY(cond)      (cond);
#endif

#define WT(what)          TR(#what":" << what) // WT: short for 'watch'
#define NOT_HERE          Q_ASSERT_X(false, "Here" , "not be!");

// to mark not yet implemented features
class QObject;
void warn(QObject*, rcstr msg, rcstr more = str::null);
inline void warn(rcstr msg, rcstr more = str::null) { warn(nullptr,msg,more); }
void notYet(rcstr = str::null);

#endif
