#include "defs.h"
#include <QDebug>

static_assert (QT_VERSION >= 0x050501,"written for Qt >= 5.5");

//------------------------------------------------------------------------------

#ifdef QT_NO_EXCEPTIONS
#error needs exception handling
#endif

// eof
