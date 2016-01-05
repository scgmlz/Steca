// common and useful definitions

#ifndef DEFS_H
#define DEFS_H

#define DEVEL

#include <QtGlobal>
#include <QString>
#include <QStringBuilder>

typedef QString     str;
typedef str const&  rcstr;

// a class definition helper: this class and superclass access
#define SUPER(cls,sup)  typedef cls thisCls; typedef sup super;

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

#endif
