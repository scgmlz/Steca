#ifndef CORE_DEBUG_H
#define CORE_DEBUG_H

#define DEVEL

// debug support
#ifndef QT_NO_DEBUG

#include <QDebug>
#define TR(what)          { qDebug() << what; } // TR: short for 'trace'
#define ASSERT(cond)      Q_ASSERT(cond);
#define NOT_HERE          Q_ASSERT_X(false, "Here" , "not be!");

#else

#define TR(what)          { }
#define ASSERT(cond)
#define NOT_HERE

#endif

#define WT(what)          TR(#what":" << what) // WT: short for 'watch'

#define RUNTIME_CHECK(test,msg)  if (!(test)) THROW(msg)

#endif
