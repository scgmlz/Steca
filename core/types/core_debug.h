// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_debug.h
//! @brief     Macros for debugging support.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_DEBUG_H
#define CORE_DEBUG_H

#ifndef QT_NO_DEBUG

#include <QDebug>

/** TRace: trace prints; takes several things separated by << e.g.
 *  TR(var)
 *  TR("name" << var1 << 2+3)
 *  Note: there must be an available QDebug& operator<<
 */
#define TR(what)      { qDebug() << what; }

/// Asserts redefined, to include (or not) the ';'

#define EXPECT(cond) EXPECT2(cond, "")
#define ENSURE(cond) ENSURE2(cond, "")

#define EXPECT2(cond, text) Q_ASSERT_X(cond, "precondition", text);
#define ENSURE2(cond, text) Q_ASSERT_X(cond, "postcondition", text);

/// Mark code that should not be reached, typically 'switch' branches
/// includes ';'

#define NEVER_HERE    Q_ASSERT_X(false, "Here", "not be!");

#else

#define TR(what)      { }

#define EXPECT(cond)
#define ENSURE(cond)

#define EXPECT2(cond, text)
#define ENSURE2(cond, text)

#define NEVER_HERE

#endif

/// WaTch: same as TR, also prints stringized version (what is being printed)
#define WT(what)      TR(#what ":" << what)

#endif // CORE_DEBUG_H
