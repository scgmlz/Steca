#ifndef CORE_DEBUG_H
#define CORE_DEBUG_H

// debug support
#define ASSERT(cond)      Q_ASSERT(cond);

#ifndef QT_NO_DEBUG
#include <QDebug>
#define TR(what)          { qDebug() << what; } // TR: short for 'trace'
#define ASSERT(cond)      Q_ASSERT(cond);
#else
#define TR(what)          { }
#define DEBUG_CODE(code)
#define ASSERT(cond)
#endif

#define WT(what)          TR(#what":" << what) // WT: short for 'watch'
#define NOT_HERE          Q_ASSERT_X(false, "Here" , "not be!");

#define RUNTIME_CHECK(test,msg)  if (!(test)) THROW(msg)

#endif
