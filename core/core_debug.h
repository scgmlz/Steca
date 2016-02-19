/** \file
 * Macros that support debugging.
 *
 * TR (TRace) and WT (WaTch) are for trace prints; e.g.
 * TR(var) TR("name" << var1 << 2+3)
 * WT(var)
 *
 * ASSERT for debug-time assertions;
 * NEVER_HERE for marking unreachable code, sometimes to satisfy the compiler;
 *          debug-time assert
 */

#ifndef CORE_DEBUG_H
#define CORE_DEBUG_H

#ifndef QT_NO_DEBUG

#include <QDebug>
#define TR(what)      { qDebug() << what; }
#define ASSERT(cond)  Q_ASSERT(cond);
#define NEVER_HERE    Q_ASSERT_X(false, "Here", "not be!");

#else

#define TR(what)      { }
#define ASSERT(cond)
#define NEVER_HERE    ;

#endif

#define WT(what)      TR(#what":" << what)
#define NOT_YET       Q_ASSERT_X(false, "Done", "this is not.");

#endif
