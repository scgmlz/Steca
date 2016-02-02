/** \file
 * Macros that support debugging.
 *
 * TR (TRace) and WT (WaTch) are for trace prints;
 * ASSERT for debug-time assertions;
 * NOT_HERE for marking unreachable code; debug-time assert
 */

#ifndef CORE_DEBUG_H
#define CORE_DEBUG_H

#ifndef QT_NO_DEBUG

#include <QDebug>
#define TR(what)          { qDebug() << what; }
#define ASSERT(cond)      Q_ASSERT(cond);
#define NOT_HERE          Q_ASSERT_X(false, "Here" , "not be!");

#else

#define TR(what)          { }
#define ASSERT(cond)
#define NOT_HERE          ;

#endif

#define WT(what)          TR(#what":" << what)

#endif
